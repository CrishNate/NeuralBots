class RGBColor
{
public:
	float r;
	float g;
	float b;
	float a;

	RGBColor(float _r, float _g, float _b, float _a = 255.0f)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
	bool operator==(RGBColor rgb)
	{
		return (rgb.r == r) || (rgb.g == g) || (rgb.b == b);
	}

	RGBColor operator/(float v)
	{
		return RGBColor(r / v, g / v, b / v);
	}

	bool Equals(RGBColor rgb)
	{
		return (r == rgb.r) && (g == rgb.g) && (b == rgb.b);
	}
};

class HSL
{
public:
	int h;
	float s;
	float l;

	HSL(int _h, float _s, float _l)
	{
		h = _h;
		s = _s;
		l = _l;
	}

	bool Equals(HSL hsl)
	{
		return (h == hsl.h) && (s == hsl.s) && (l == hsl.l);
	}
};

static float HueToRGB(float v1, float v2, float vH) {
	if (vH < 0)
		vH += 1;

	if (vH > 1)
		vH -= 1;

	if ((6 * vH) < 1)
		return (v1 + (v2 - v1) * 6 * vH);

	if ((2 * vH) < 1)
		return v2;

	if ((3 * vH) < 2)
		return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

	return v1;
}

static RGBColor HSLToRGB(HSL hsl) 
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;

	if (hsl.s == 0)
	{
		r = g = b = (unsigned char)(hsl.l * 255);
	}
	else
	{
		float v1, v2;
		float hue = (float)hsl.h / 360;

		v2 = (hsl.l < 0.5) ? (hsl.l * (1 + hsl.s)) : ((hsl.l + hsl.s) - (hsl.l * hsl.s));
		v1 = 2 * hsl.l - v2;

		r = (unsigned char)(255 * HueToRGB(v1, v2, hue + (1.0f / 3)));
		g = (unsigned char)(255 * HueToRGB(v1, v2, hue));
		b = (unsigned char)(255 * HueToRGB(v1, v2, hue - (1.0f / 3)));
	}

	return RGBColor(r, g, b);
}