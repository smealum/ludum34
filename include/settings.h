#ifndef SETTINGS_H
#define SETTINGS_H

class Settings
{
	public:
		Settings(int _width, int _height);

		inline float getAspectRatio()
		{
			return ((float)width) / height;
		}

	public:
		int width, height;
};

extern Settings settings;

#endif
