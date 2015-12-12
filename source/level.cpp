#include "level.h"

SliceCollection::SliceCollection():
	cubes(SC_NUMCUBES, 0),
	depth(0)
{

}

void SliceCollection::addSlice(slice_s s)
{
	for(int i = 0; i < LEVEL_WIDTH; i++)
	{
		for(int j = 0; j < LEVEL_WIDTH; j++)
		{
			if(s.data[i][j])
			{
				cubes.addCube(glm::vec3(i * 1.0f, j * 1.0f, depth * 1.0f), glm::vec3(1.0f, 0.0f, 1.0f));
			}
		}
	}

	depth++;
	data.push_back(s);
	cubes.update();
}

void SliceCollection::popSlice()
{
	int slice_depth = depth - data.size();

	cubes.removeDepth(float(slice_depth));

	data.pop_front();
}

unsigned char mergeCubes(unsigned char c1, unsigned char c2)
{
	return c1 ? c1 : c2;
}

void initSlice(slice_s* s)
{
	if(!s) return;

	memset(s, 0, sizeof(slice_s));
}

void mergeSlices(slice_s* s1, slice_s* s2)
{
	if(!s1 || !s2) return;

	for(int i = 0; i < LEVEL_WIDTH; i++)
	{
		for(int j = 0; j < LEVEL_WIDTH; j++)
		{
			s1->data[i][j] = mergeCubes(s1->data[i][j], s2->data[i][j]);
		}
	}
}

void SliceCollection::clear()
{
	data.clear();
	cubes.clear();
	depth = 0;
}

void SliceCollection::mergeLayers(SliceCollection** sc, int n)
{
	if(!sc) return;

	clear();

	std::deque<slice_s>::iterator its[n];

	for(int i = 0; i < n; i++)
	{
		its[i] = sc[i]->data.begin();
	}

	while(true)
	{
		slice_s dst_slice;

		initSlice(&dst_slice);

		bool done = true;
		for(int i = 0; i < n; i++)
		{
			if(its[i] != sc[i]->data.end())
			{
				mergeSlices(&dst_slice, &*its[i]);
				++its[i];
				done = false;
			}
		}
		if(done) break;

		addSlice(dst_slice);
	}
}

void SliceCollection::draw(Camera& camera, Lighting& lighting)
{
	cubes.model = glm::translate(glm::mat4(1.0f), glm::vec3(-LEVEL_WIDTH / 2, -LEVEL_WIDTH / 2, 0.0f));
	cubes.draw(camera, lighting);
}

Layer::Layer()
{

}

void Layer::draw(Camera& camera, Lighting& lighting)
{
	slices.draw(camera, lighting);
}

void Layer::addSlice(slice_s s)
{
	slices.addSlice(s);
}

Level::Level()
{

}

void Level::draw(Camera& camera, Lighting& lighting)
{
	slices.draw(camera, lighting);
}

void Level::update()
{
	SliceCollection* sc[LEVEL_NUMLAYERS];

	for(int i = 0; i < LEVEL_NUMLAYERS; i++) sc[i] = &layers[i].slices;

	slices.mergeLayers(sc, LEVEL_NUMLAYERS);
}

void Level::addSliceLayer(int l, slice_s s, bool update)
{
	if(l < 0 || l >= LEVEL_NUMLAYERS) return;

	layers[l].addSlice(s);

	if(update) this->update();
}
