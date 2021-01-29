

vec2 planarTextureMap(vec4 pos)
{
	return pos.xy;
}

vec2 cylindricalTextureMap(vec4 pos)
{
	float theta = degrees(atan(pos.z, pos.x));
	return vec2(theta / 360, pos.y);
}

vec2 sphericalTextureMap(vec4 pos)
{
	float theta = degrees(atan(pos.z, pos.x));
	float r = length(pos);//sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
	float phi = degrees(acos(pos.y / r));
	
	return vec2(theta / 360, phi / 180);
}

vec2 cubicTextureMap(vec4 r, out uint cubeFace)
{
	r = normalize(r);
	if(abs(r.x) > abs(r.y) && abs(r.x) > abs(r.z))
	{
		cubeFace = (r.x > 0)? 0 : 2;
		r *= 1.f / abs(r.x);
		r /= 2;
		r += vec4(0.5, 0.5, 0.5, 0);
		return r.yz;
	}
	else if(abs(r.y) > abs(r.x) && abs(r.y) > abs(r.z))
	{
		cubeFace = (r.y > 0)? 4 : 5;
		r *= 1.f / abs(r.y);
		r /= 2;
		r += vec4(0.5, 0.5, 0.5, 0);
		return r.xz;
	}
	else if(abs(r.z) > abs(r.x) && abs(r.z) > abs(r.y))
	{
		cubeFace = (r.z > 0)? 1 : 3;
		r *= 1.f / abs(r.z);
		r /= 2;
		r += vec4(0.5, 0.5, 0.5, 0);
		return r.xy;
	}
}

vec2 calculateUV(vec4 pos, vec2 uv_frag, int textureMode_)
{
	switch(textureMode_)
	{
	case 0: // CPU
		return uv_frag;
	case 1: // GPU_PLANAR
		return planarTextureMap(pos);
	case 2: // GPU_CYLINDRICAL
		return cylindricalTextureMap(pos);
	case 3: // GPU_SPHYRICAL
		return sphericalTextureMap(pos);
	}
}