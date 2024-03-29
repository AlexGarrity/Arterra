#include "world/Chunk.hpp"

#include "world/Block.hpp"
#include "world/SubChunk.hpp"
#include "world/World.hpp"

namespace arterra {

	Chunk::Chunk(int posX, int posZ, World* world)
		: _position { posX, posZ }
		, _world { world }
	{
	}

	Chunk::Chunk(World* world)
		: Chunk(0, 0, world)
	{
	}

	Chunk::Chunk(const Chunk& other)
	{
		_position = other._position;
		_world = other._world;
		_subChunks = std::move(other._subChunks);
		for (auto& sc : _subChunks) {
			sc.second->SetParent(this);
		}
	}

	Chunk::~Chunk()
	{
		for (auto& sc : _subChunks) {
			if (sc.second)
				delete sc.second;
		}
	}

	void Chunk::SetPosition(int x, int z)
	{
		_position._x = x;
		_position._z = z;
	}

	void Chunk::SetWorld(World* world) { _world = world; }

	void Chunk::CreateSubChunk(int y) { CreateSubChunkCS(y / SubChunk::SIZE_Y); }

	void Chunk::CreateSubChunksToHeight(int height) { CreateSubChunksToHeightCS(height / SubChunk::SIZE_Y); }

	SubChunk* Chunk::GetSubChunk(int y) { return GetSubChunkCS(y / SubChunk::SIZE_Y); }

	void Chunk::CreateSubChunkCS(int y)
	{
		if (_subChunks.find(y) != _subChunks.end())
			return;
		_subChunks.emplace(y, new SubChunk(y, this));
	}

	void Chunk::CreateSubChunksToHeightCS(int height)
	{
		for (auto iY = 0; iY <= height; ++iY) {
			CreateSubChunkCS(iY);
		}
	}

	SubChunk* Chunk::GetSubChunkCS(int y)
	{
		auto it = _subChunks.find(y);
		if (it == _subChunks.end())
			return nullptr;
		return it->second;
	}

	void Chunk::UpdateNeighbours()
	{
		Chunk* n;
		
		n = _world->GetChunkCS(_position._x + 1, _position._z);
		if (n)
			n->UpdateBorder(Direction::NegX);

		n = _world->GetChunkCS(_position._x - 1, _position._z);
		if (n)
			n->UpdateBorder(Direction::PosX);

		n = _world->GetChunkCS(_position._x, _position._z + 1);
		if (n)
			n->UpdateBorder(Direction::NegZ);

		n = _world->GetChunkCS(_position._x, _position._z - 1);
		if (n)
			n->UpdateBorder(Direction::PosZ);
	}

	void Chunk::UpdateBlocks()
	{
		for (auto& sc : _subChunks) {
			for (auto b : sc.second->GetBlocks()) {
				if (b)
					b->Update(0);
			}
		}
	}

	void Chunk::UpdateBorder(Direction borderDirection)
	{
		switch (borderDirection) {
			case Direction::PosY: {
				Logger::Debug( "Updating positive Y border for chunk at (", _position._x, ", ",
					_position._z, ").  Whilst possible, was this intended?");
				SubChunkMap::iterator highestSC;
				for (auto it = _subChunks.begin(); it != _subChunks.end(); ++it) {
					if (it->second->GetPositionRaw() > highestSC->second->GetPositionRaw()) {
						highestSC = it;
					}
				}
				highestSC->second->UpdateBorder(PosY);
			} break;

			case Direction::NegY: {
				Logger::Debug( "Updating negative Y border for chunk at (", _position._x, ", ",
					_position._z, ").  Whilst possible, was this intended?");
				SubChunkMap::iterator lowestSC;
				for (auto it = _subChunks.begin(); it != _subChunks.end(); ++it) {
					if (it->second->GetPositionRaw() > lowestSC->second->GetPositionRaw()) {
						lowestSC = it;
					}
				}
				lowestSC->second->UpdateBorder(PosY);
			} break;

			default:
				for (auto& sc : _subChunks) {
					sc.second->UpdateBorder(borderDirection);
				}
				break;
		}
	}

	void Chunk::UpdateSubChunkBorder(int y, Direction borderDirection)
	{
		int scY = y / SubChunk::SIZE_Y;
		UpdateSubChunkBorderSC(scY, borderDirection);
	}

	void Chunk::UpdateSubChunkBorderSC(int y, Direction borderDirection)
	{
		auto it = _subChunks.find(y);
		if (it == _subChunks.end())
			return;
		it->second->UpdateBorder(borderDirection);
	}

	SubChunkMap& Chunk::GetSubChunks() { return _subChunks; }

	WorldPosition Chunk::GetPosition()
	{
		return { _position._x * SubChunk::SIZE_X, 0, _position._z * SubChunk::SIZE_Z };
	}

	ChunkPosition Chunk::GetPositionRaw() { return _position; }

	std::vector<SubChunk*> Chunk::Update(float deltaTime)
	{
		std::vector<SubChunk*> out;
		out.reserve(6);
		for (auto sc : _subChunks) {
			if (!sc.second)
				continue;
			if (sc.second->IsUpdated()) {
				sc.second->SetUpdated(false);
				out.push_back(sc.second);
			}
		}
		return { out.begin(), out.end() };
	}

	World* Chunk::GetWorld() { return _world; }

}
