#pragma once

#include "PCH.hpp"

#include "gamestate/Base.hpp"

#include "world/World.hpp"
#include "model/CullableModel.hpp"
#include "renderer/ChunkRenderer.hpp"
#include "renderer/ShaderManager.hpp"
#include "world/TerrainGenerator.hpp"
#include "block/BlockManager.hpp"
#include "texture/TextureAtlas.hpp"
#include "texture/Texture.hpp"

namespace arterra {


	namespace gamestate {

		class Test : public Base {
		public:
			Test(Engine* engine);

			virtual void Input(float_t deltaTime) override;
			virtual void Update(float_t deltaTime) override;
			virtual void Render(float_t deltaTime) override;

		private:
			float_t _timeToResourceUnload = 5.0f;
			CullableModel _blockModel;
			World _world;

			ChunkRenderer _chunkRenderer;
			ShaderManager _shaderManager;
			TerrainGenerator _terrainGenerator;
			BlockManager _blockManager;
			TextureAtlas _atlas;

			// Temporary player controller.
			float_t _speed = 2.0f;
			float_t _rotSpeed = glm::radians(15.0f);
		};

	}

}