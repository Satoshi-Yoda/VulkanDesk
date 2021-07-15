#include "Scene.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../../VulkanCore/utils/Loader.h"

using namespace std;

Scene::Scene(Ash& ash, Batcher& batcher, Mountain& mountain, Rocks& rocks, Crater& crater, Lava& lava) : ash(ash), batcher(batcher), mountain(mountain), rocks(rocks), crater(crater), lava(lava) {}

Scene::~Scene() {}

void Scene::init() {
	vec4 color { 0.9f, 0.7f, 0.5f, 0.5f };
	vec2 size { 32, 240 };
	float radius = 4.0f;
	float y = 200;

	shared_ptr<Rectangle> rect1 = make_shared<Rectangle>(ash, mountain, rocks, crater, lava);
	rect1->data.color = color;
	rect1->data.size = size;
	rect1->data.radius = radius;
	rect1->data.aa = 0.0f;
	rect1->position = { -size.x * 1.5, y };
	rect1->paint();

	shared_ptr<Rectangle> rect2 = make_shared<Rectangle>(ash, mountain, rocks, crater, lava);
	rect2->data.color = color;
	rect2->data.size = size;
	rect2->data.radius = radius;
	rect2->data.aa = 0.5f;
	rect2->position = { 0, y };
	rect2->paint();

	shared_ptr<Rectangle> rect3 = make_shared<Rectangle>(ash, mountain, rocks, crater, lava);
	rect3->data.color = color;
	rect3->data.size = size;
	rect3->data.radius = radius;
	rect3->data.aa = 0.8f;
	rect3->position = { +size.x * 1.5, y };
	rect3->paint();

	shared_ptr<Rectangle> rect4 = make_shared<Rectangle>(ash, mountain, rocks, crater, lava);
	rect4->data.color = color;
	rect4->data.size = size;
	rect4->data.radius = radius;
	rect4->data.aa = 1.0f;
	rect4->position = { +size.x * 1.5 * 2, y };
	rect4->paint();

	shared_ptr<Rectangle> rect5 = make_shared<Rectangle>(ash, mountain, rocks, crater, lava);
	rect5->data.color = color;
	rect5->data.size = size;
	rect5->data.radius = radius;
	rect5->data.aa = 1.5f;
	rect5->position = { +size.x * 1.5 * 3, y };
	rect5->paint();

	shared_ptr<Graphic> gr = make_shared<Graphic>(ash, mountain, rocks, crater, lava);
	gr->data.bgColor = color;
	gr->data.lineColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	gr->data.size = { 800, 400 };
	gr->data.radius = 20.0f;
	gr->data.aa = 1.0f;
	gr->data.points = { 0.1f, 0.4f, 0.2f, 0.5f, 0.4f };
	gr->position = { -350, -200 };
	gr->paint();

	/*
	1) make refresh() and erase() somehow
	2) refactor to following state:

	shared_ptr<Graphic> graphic = make_shared<Graphic>(ash, mountain, ...);
	graphic->bgColor = ...
	graphic->size = ...
	graphic->paint();

	graphic->size = ...
	graphic->refresh();

	graphic->erase();
	*/

	shared_ptr<Rectangle> rect_b = make_shared<Rectangle>(ash, mountain, rocks, crater, lava);
	rect_b->data.color = color;
	rect_b->data.size = { 400, 400 };
	rect_b->data.radius = 20.0f;
	rect_b->data.aa = 1.0f;
	rect_b->position = { 300, -200 };
	rect_b->paint();

	// "asteroid-s1.1"
	// printf("Lava: sprite %.0fx%.0f\n", round(width * scale), round(height * scale));

	extent_h = 800 / 2;
	extent_w = 1500 / 2;

	// full scale
	// N = 1840000;   // static
	// N = 1700000; // old stream with instances (92% from static) 780 Mb/second
	N = 200; // new stream with instances
	// N = 100000; // dynamic 10% with instances
	float percent = 0.05;
	// N = 301;

	// 0.5 scale
	// N = 5888000;
	// N = 4500000; // stream size 0.5 (76% from static) 2070 Mb/second

	// 0.25 size
	// N = 11905000;
	// N = 4875000; // stream size 0.5 (41% from static) 2240 Mb/second

	size_t count = 0.97 * sqrt(2 * N) * extent_h / extent_w;
	float step = 2.0f * extent_h / count;

	for (float x = -extent_w; x < extent_w; x += step)
	for (float y = -extent_h; y < extent_h; y += step)
	{
		Instance instance { { x, y } };
		size_t index = batcher.addInstance("bomb.6", instance);
		instances[index] = instance;
		if (distribution(random) < percent) {
			updatableIndexes.push_back(index);
		}
	}

	for (float x = -extent_w; x < extent_w; x += step)
	for (float y = -extent_h; y < extent_h; y += step)
	{
		Instance instance { { x + step / 2, y + step / 2 } };
		batcher.addInstance("asteroid-s3.2", instance);
	}

	// printf("Lava: %lld sprites\n", instances.size());
	// printf("Lava: stream: %.2f Mb/frame\n", static_cast<float>(instances.size() * sizeof(Instance)) / (1 << 20));
	// printf("Lava: stream: %.2f Mb/second (for 60 fps)\n", 60 * static_cast<float>(instances.size() * sizeof(Instance)) / (1 << 20));
	// printf("Lava: fillrate: %.0f Mpixels/frame\n", (instances.size()) * width * height * scale * scale / 1000000);
}

void Scene::update(double t, double dt) {
	for (auto i : updatableIndexes) {
		float add = 40 * cos(t) * dt;
		vec2 addv { add, add };
		instances[i].pos = instances[i].pos + addv;
		batcher.updateInstance("bomb.6", i, instances[i]);
	}

	if (t >= nextChange) {
		uniform_int_distribution<size_t> indexDistribution { 0, static_cast<size_t>(N * 2.5) };

		for (int i = 0; i < 100000; i++) {
			size_t index = indexDistribution(random);

			if (instances.find(index) != instances.end())
			if (find(updatableIndexes.begin(), updatableIndexes.end(), index) == updatableIndexes.end())
			{
				instances.erase(index);
				batcher.removeInstance("bomb.6", index);
				break;
			}
		}
	}

	if (t >= nextChange) {
		uniform_int_distribution<int> xDistribution { -extent_w, extent_w };
		uniform_int_distribution<int> yDistribution { -extent_h, extent_h };

		Instance instance { { xDistribution(random), yDistribution(random) } };
		size_t index = batcher.addInstance("bomb.6", instance);
		instances[index] = instance;

		nextChange = nextChange + 1.0/60.0;
	}

	// TODO
	// so, if changed about 1% or less, then updateInstances(), else updateInstanceBuffer()
	// and updateInstances() loads CPU, but updateInstanceBuffer() loads PCI-E,
	// lava.updateInstances(lavaObjectId, instances, updatableIndexes);
	// lava.updateInstanceBuffer(lavaObjectId, instances);
}

size_t Scene::sprites() {
	return instances.size();
}
