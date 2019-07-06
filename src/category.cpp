
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#include "category.h"
#include "resourceobject.h"

#include <vector>
#include <wx/image.h>
using namespace std;


static const vector<string> c_main {
	"airplane",			"bicycle",		"clock",			"dolphin",		"earth",
	"fire",				"grasshopper",	"hat",				"igloo",		"jar",
	"keyboard",			"ladybug",		"magnifying glass",	"newspaper",	"onion",
	"paper airplane",	"quarter",		"raccoon",			"star",			"train",
	"umbrella",			"vase",			"watch",			"x-ray",		"yinyang",
	"zipper",
};

static const vector<string> c_food {
	"apple",	"bananas",		"cherries",		"doughnut",		"egg",
	"fries",	"grapes",		"hot dog",		"ice cream",	"juice",
	"ketchup",	"lasagna",		"mushroom",		"noodle",		"olive",
	"pretzel",	"quiche",		"radishes",		"strawberry",	"tomato",
	"udon",		"vegetables",	"watermelon",	"x-cookie",		"yam",
	"zucchini",
};

static const vector<string> c_animal {
	"angelfish",	"bird",		"cat",		"dog",		"elephant",
	"frog",			"giraffe",	"horse",	"iguana",	"jaguar",
	"koala",		"lion",		"mouse",	"newt",		"owl",
	"pig",			"quail",	"rooster",	"sheep",	"turtle",
	"urchin",		"vulture",	"whale",	"xiphias",	"yak",
	"zebra",
};

static const vector<string> c_music {
	"accordion",	"bagpipes",	"clarinet",	"drums",		"electric guitar",
	"flute",		"guitar",	"harp",		"ipu",			"jug",
	"kazoo",		"lute",		"maracas",	"ney",			"oboe",
	"piano",		"quena",	"recorder",	"saxophone",	"trumpet",
	"ukulele",		"violin",	"whistle",	"xun",			"yangqin",
	"zurna",
};

static const vector<string> c_toy {
	"anklet",	"balloons",		"chalkboard",		"doll",			"easel",
	"football",	"glove",		"horn",				"ice skate",	"jump rope",
	"kite",		"lunchbox",		"magnet",			"necklace",		"origami",
	"paint",	"quilt",		"rocking horse",	"seesaw",		"top",
	"unicycle",	"volleyball",	"wagon",			"xylophone",	"yoyo",
	"zipline",
};

ResourceList createCategory(string cat_name) {
	vector<string> category;

	if (cat_name == "food") {
		category = c_food;
	} else if (cat_name == "animal") {
		category = c_animal;
	} else if (cat_name == "music") {
		category = c_music;
	} else if (cat_name == "toy") {
		category = c_toy;
	} else {
		cat_name = "main";
		category = c_main;
	}

	ResourceList resList = ResourceList();
	for (unsigned int idx = 0; idx < category.size(); idx++) {
		resList.add(ResourceObject(wxString(category.at(idx)), cat_name));
	}

	return resList;
}

ResourceList createCategory(wxString cat_name) {
	return createCategory(string(cat_name.mb_str()));
}
