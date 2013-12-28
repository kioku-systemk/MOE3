#ifndef INCLUDE_MOE_IMAGE_H
#define INCLUDE_MOE_IMAGE_H

namespace MOE {
	class Stream;

class Image
{
private:
	class Impl;
	Impl* m_imp;
		
public:
	Image(void);
	~Image(void);

	bool Load(const char* fname);
	bool Load(const MOE::Stream* s);
    int* GetImage() const;
	
	int GetWidth() const;
	int GetHeight() const;
};

}
#endif // INCLUDE_MOE_IMAGE_H

