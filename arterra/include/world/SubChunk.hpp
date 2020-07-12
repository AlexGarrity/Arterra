#pragma once

#include "PCH.hpp"
#include "world/Block.hpp"

namespace arterra {

	class Chunk;

	class SubChunk {

	public:
		static const size_t SIZE_X = 16;
		static const size_t SIZE_Y = 16;
		static const size_t SIZE_Z = 16;
		static const size_t SIZE = SIZE_X * SIZE_Y * SIZE_Z;

		SubChunk(int posX, int posY, int posZ, Chunk* parent);
		SubChunk(const SubChunk& other);
		~SubChunk() {}
		void SetParent(Chunk* chunk);

		std::array<Block*, SIZE>& GetBlocks();
		Block* GetBlock(int x, int y, int z) const;

		void SetBlock(int x, int y, int z, BlockData &data);
		void DeleteBlock(int x, int y, int z);

		BlockPosition GetPosition();
		inline Chunk* GetChunk() { return _chunk; };

		bool Update(float deltaTime);

	private:
		BlockPosition _position;
		Chunk* _chunk;
		bool _updated = true;

		// Contains 16 x 16 x 16 blocks.
		std::array<Block*, SIZE> _blocks;
	};

}