#pragma once

#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "../../VulkanCore/batch/Batcher.h"
#include "../../VulkanCore/batch/BatchLayout.h"
#include "../../VulkanCore/core/Lava.h"
#include "../../VulkanCore/core/Tectonic.h"
#include "../../VulkanCore/graphic/Graphic.h"
#include "../../VulkanCore/graphic/GraphicLayout.h"
#include "../../VulkanCore/rectangle/Rectangle.h"
#include "../../VulkanCore/rectangle/RectangleLayout.h"

using glm::vec2;
using std::mt19937_64;
using std::string;
using std::uniform_real_distribution;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

class Scene {
public:
	Scene(Ash& ash, Batcher& batcher, Mountain& mountain, Rocks& rocks, Crater& crater, Lava& lava);
	~Scene();

	void init();
	void update(double t, double dt);
	size_t sprites();

private:
	Ash& ash;
	Batcher& batcher;
	Mountain& mountain;
	Rocks& rocks;
	Crater& crater;
	Lava& lava;

	unordered_map<size_t, Instance> instances;
	vector<size_t> updatableIndexes;
	shared_ptr<Rectangle> rect;

	mt19937_64 random;
	uniform_real_distribution<double> distribution { 0.0, 1.0 };
	size_t N;
	int extent_w, extent_h;
	double nextChange = 0.0f;

	unique_ptr<Rectangle> initRectangle();
	vector<Vertex> initQuad(uint32_t w, uint32_t h);
};
