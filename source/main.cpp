#include <3ds.h>
#include <citro2d.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <vector>
#include <random>
#include <algorithm>

#include "app/app.h"

int main()
{
	App app;

	while (aptMainLoop())
	{
		// game loop
		if (!app.frame())
			break;
	}

	return 0;
}