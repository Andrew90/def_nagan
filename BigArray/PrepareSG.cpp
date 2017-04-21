#include "PrepareSG.h"
#include "Lir.h"

#include "DebugMess.h"

PrepareSG::PrepareSG()
{
}


PrepareSG::~PrepareSG()
{
}

void PrepareSG::Do(unsigned long *Base, int len, unsigned *time, vector<int> (&res))
{
	Lir &lir = Lir::Instance();
	unsigned timeStart = lir.sGUnit.bitTimeUp;
	unsigned timeStop = lir.sGUnit.bitTimeDown;
	int k = 0;
	unsigned o0 = 0;
	int maxFrames = len / buffer_size;
	for (; k < 512 && k < maxFrames; ++k)
	{
		if (timeStart < time[k])
		{
			o0 = int(((double)(k - 1) + (double)(timeStart - time[k - 1]) / (time[k] - time[k - 1])) * buffer_size / count_sensors);
			break;
		}
	}
	unsigned o1 = 0;
	for (; k < 512 && k < maxFrames; ++k)
	{
		if (timeStop < time[k])
		{
			o1 = int(((double)(k - 1) + (double)(timeStop - time[k - 1]) / (time[k] - time[k - 1])) * buffer_size / count_sensors);
			break;
		}
	}

	int offs = 10 * (o1 - o0) / 100;

	int start = o0 + offs;
	int stop =  o1 - offs;

	res.resize((stop - start) * 2);

	int j = 0;
	int kk = 0;
	for (int i = start; i < stop; ++i)
	{
		j = i * count_sensors;

		short t = Base[j - 2];
		res[kk] = t;
		++kk;
		t = Base[j - 1];
		res[kk] = t;
		++kk;
	}
}

void PrepareSG::TestDo(unsigned long *Base, int len, unsigned *time, std::vector<int> (&res))
{
	unsigned start = 0;
	unsigned stop = len / count_sensors;
	res.resize((stop - start) * 2);

    int j = 0;
	int kk = 0;
	for (int i = start; i < stop; ++i)
	{
		j = i * count_sensors;

		short t = Base[j - 2];
		res[kk] = t;
		++kk;
		t = Base[j - 1];
		res[kk] = t;
		++kk;
	}
}

PrepareSG &PrepareSG::Instance()
{
	static PrepareSG x; return x;
}