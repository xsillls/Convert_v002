#include "Menu.h"


int loads = -1;
int index_animation = 0;

bool TickDelay(int max_ticks)
{
	static int tick = 0;

	tick++;

	if (tick >= max_ticks)
	{
		tick = 0;
		return true;
	}

	return false;
}

static char load_1[4] = {
	'\\',
	'|',
	'/',
	'-'
};
static std::string dots[4] = {
    ".",
    "..",
    "...",
    "...."
};

std::string Logo_Anim(std::string Logos, int seting)
{
    if (TickDelay(5))
    {
        loads += 1;
        index_animation += 1;
    }

    std::string Logo = "CONVERTER " + Logos;

    if (index_animation < 0)
    {
        return Logo.substr(0, loads);
    }

    if (index_animation == 4)
    {
        index_animation = 0;
    }

    if (seting == 0)
        return Logo.substr(0, loads) + " " + std::string(1, load_1[index_animation]);
    if (seting == 1)
        return Logo.substr(0, loads) + dots[index_animation];

}
