#include "block/BlockManager.hpp"

#include "block/BlockData.hpp"

namespace arterra {

	BlockManager::BlockManager() {
		_blocks.reserve(128);
	}

	BlockManager::~BlockManager() {
		for (auto x : _blocks) {
			delete x;
		}
	}

	bool BlockManager::AddBlock(BlockData &&blockData, std::string identifier) {
		if (_blockMapper.find(identifier) != _blockMapper.end()) return false;
		auto index = _blocks.size();
		_blocks.emplace_back(new BlockData(blockData));
		_blockMapper[identifier] = index;
		return true;
	}

	bool BlockManager::RemoveBlock(std::string identifier) {
		auto it = _blockMapper.find(identifier);
		if (it == _blockMapper.end()) return false;
		auto index = it->second;
		_blocks.erase(_blocks.begin() + index);
		_blockMapper.erase(it);
		return true;
	}

	BlockData *BlockManager::GetBlock(std::string identifier) {
		auto it = _blockMapper.find(identifier);
		if (it == _blockMapper.end()) return nullptr;
		return _blocks[it->second];
	}

	BlockData *BlockManager::GetBlock(size_t index) {
		if (index < _blocks.size()) return nullptr;
		return _blocks[index];
	}

}