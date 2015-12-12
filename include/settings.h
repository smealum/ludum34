#ifndef SETTINGS_H
#define SETTINGS_H

class Settings
{
	public:
		Settings(int _width, int _height, float _line_width = 4.0f);

		inline float getAspectRatio()
		{
			return ((float)width) / height;
		}

	public:
		int width, height;
		float line_width;
};

extern Settings settings;

#endif
