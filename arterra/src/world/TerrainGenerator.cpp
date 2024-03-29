#include "world/TerrainGenerator.hpp"

#include "block/BlockManager.hpp"
#include "thread/ThreadManager.hpp"
#include "thread/ThreadJob.hpp"

#include "world/Block.hpp"
#include "world/Chunk.hpp"
#include "world/SubChunk.hpp"

#include <glm/gtc/noise.hpp>

namespace arterra {

	TerrainGenerator::TerrainGenerator(ThreadManager* threadManager, BlockManager* blockManager)
		: _threadManager{ threadManager },
		  _blockManager{ blockManager }
		  , _exiting{ false }
	{
		std::srand(std::time(nullptr));
		seedX = rand() % 65536;
		seedY = rand() % 65536;
		seedZ = rand() % 65536;
	}

	TerrainGenerator::~TerrainGenerator() { }

	std::vector<Chunk*>& TerrainGenerator::GetCompletedChunks() { return _completedChunks; }

	void TerrainGenerator::AddChunkToGeneratorQueue(Chunk* chunk)
	{
		if (!chunk)
			return;
		_pendingChunks.emplace(chunk);
		ThreadJob j([this, chunk]()
		{
			GenerateChunk(*chunk);
			MarkChunkAsCompleted(chunk);
		});
		_threadManager->PushJob(j);
	}

	void TerrainGenerator::MarkChunkAsCompleted(Chunk* c)
	{
		const auto it = _pendingChunks.find(c);
		_pendingChunks.erase(it);
		_completedChunks.emplace_back(c);
	}

	void TerrainGenerator::GenerateChunk(Chunk& out)
	{
		
		std::vector<uint16_t> _heightMap;
		_heightMap.resize(256);
		auto cp = out.GetPosition();
		for (auto z = 0; z < SubChunk::SIZE_Z; ++z) {
			for (auto x = 0; x < SubChunk::SIZE_X; ++x) {
				auto fX = static_cast<float>(x + cp._x + seedX) / 64.0f;
				auto fZ = static_cast<float>(z + cp._z + seedZ) / 64.0f;
				float g1 = glm::simplex(glm::vec2(fX, fZ)) * 0.0625f;
				float g2 = glm::simplex(glm::vec2(fX * 0.25f, fZ * 0.25f)) * 0.25f;
				float g3 = glm::simplex(glm::vec2(fX * 0.125f, fZ * 0.125f)) * 0.5f;
				float go = g1 + g2 + g3 + 1.0f;
				uint16_t h = static_cast<uint16_t>(go * 64.0f);
				_heightMap[x + z * SubChunk::SIZE_X] = h;
			}
		}

		size_t largestHeight = 0;
		for (auto x : _heightMap) {
			if (x > largestHeight)
				largestHeight = x;
		}

		out.CreateSubChunksToHeight(largestHeight);

		auto grassBlock = _blockManager->GetBlock("grass");
		auto dirtBlock = _blockManager->GetBlock("dirt");
		auto stoneBlock = _blockManager->GetBlock("stone");

		for (auto& isc : out.GetSubChunks()) {
			auto sc = isc.second;
			for (auto z = 0; z < SubChunk::SIZE_Z; ++z) {
				for (auto x = 0; x < SubChunk::SIZE_X; ++x) {
					auto height = _heightMap[x + SubChunk::SIZE_X * z];
					auto pos = sc->GetPosition();
					// std::max / std::min can't take a reference to a static const (by design),
					// so use this little workaround
					auto maxHeight = SubChunk::SIZE_Y;
					auto dY = std::max<int>(std::min<int>(height - pos._y, maxHeight), 0);
					for (auto y = 0; y < dY; ++y) {
						if (pos._y + y < height - 3)
							sc->SetBlockCS(x, y, z, *stoneBlock);
						else if (pos._y + y < height - 1)
							sc->SetBlockCS(x, y, z, *dirtBlock);
						else
							sc->SetBlockCS(x, y, z, *grassBlock);
					}
				}
			}
		}
		out.UpdateBlocks();
		out.UpdateNeighbours();
	}

	void TerrainGenerator::GenerateBlock(int x, int y, int z, Block& block) {}

}
