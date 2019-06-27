#include "category.h"
#include "resourceobject.h"

#include <vector>
#include <wx/image.h>
#include <wx/string.h>
using namespace std;


// FIXME: need to use directory prefixes for main group
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
	"udon",		"vegetables",	"watermelon",	"x cookie",		"yam",
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

ResourceList* createCategory(string catName) {
	vector<string> category;

	if (catName == "food") {
		category = c_food;
	} else if (catName == "animal") {
		category = c_animal;
	} else if (catName == "music") {
		category = c_music;
	} else if (catName == "toy") {
		category = c_toy;
	} else {
		category = c_main;
	}

	// XXX: Why does this have to be a pointer?
	ResourceList* resList = new ResourceList();

	for (unsigned int idx = 0; idx < category.size(); idx++) {
		const wxString label = wxString(category.at(idx));
		const wxString img = wxString(catName).Append("/").Append(label);
		const wxString snd = label;
		resList->add(ResourceObject(label, img, snd));
	}

	return resList;
}
