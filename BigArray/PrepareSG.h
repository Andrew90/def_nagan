#ifndef PrepareSG_h
#define  PrepareSG_h
#include <vector>
class PrepareSG
{
public:
	static const int count_sensors = 18;     //количество датчиков
	static const int buffer_size   = 65536; //размер буфера 791
	PrepareSG();
	~PrepareSG();
	void Do(unsigned long *Base, int len, unsigned *time, std::vector<int> (&));
	void TestDo(unsigned long *Base, int len, unsigned *time, std::vector<int> (&));
	static PrepareSG &Instance();
};
#endif

