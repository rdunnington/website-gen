#include <stdio.h>

struct Resource
{
	int value;
};

void ResourceInitialize(struct Resource* r)
{
	r->value = 0x1138;
}

void ResourceCleanup(struct Resource* r)
{
	r->value = 0;
}

#define SCOPED_RESOURCE_BEGIN(name)		\
	struct Resource name;				\
	ResourceInitialize(&name);			\
	int name ## _END_SCOPE_NOT_FOUND;

#define SCOPED_RESOURCE_END(name)		\
    ResourceCleanup(&name);				\
    (void)name ## _END_SCOPE_NOT_FOUND;

int main(void)
{
	SCOPED_RESOURCE_BEGIN(my_resource);

	printf("my_resource->value: 0x%x\n", my_resource.value);

	// Uncomment this to fix the compile error
	//SCOPED_RESOURCE_END(my_resource);

	return 0;
}
