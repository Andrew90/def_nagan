#include "Lir.h"
#include "ItemIni.h"
#include "DebugMess.h"


static wchar_t *fileName = L"..\\..\\Settings\\Lir.ini";
wchar_t *Lir_inputs = L"INPUTS";
wchar_t *Lir_description = L"DESCRIPTION";

CntrEventHandler::CntrEventHandler(Lir &o)
	: h(o.h)
	, tabIndex(o.tabIndex)
	, tabIndex2(o.tabIndex2)
{}

void BDAQCALL CntrEventHandler::UdCntrEvent(void * sender, Automation::BDaq::tagUdCntrEventArgs * args)
{
	switch(args->SrcId)
	{
	case EvtCntPatternMatch0://событие 1 лира
		//  case EvtCntPatternMatch2://событие 1 лира
		tabIndex = ((Automation::BDaq::UdCounterCtrl *)sender)->getValue();//текущее значение счётчика лира
   		lastCounter = 0;
		SetEvent(h);
		break;
	case EvtCntPatternMatch1://событие 2 лира
		//   case EvtCntPatternMatch3://событие 2 лира
		{
			unsigned t = ((Automation::BDaq::UdCounterCtrl *)sender)->getValue();
			tabIndex += t - lastCounter;//прибавляется приращение второго лира к текущему значению
			lastCounter = t;
			tabIndex2 = t;
			SetEvent(h);
		}
		break;

	}
}
   
Lir::Lir(void)
	: crossUnit(*this)
	, thicknessUnit(*this)
	, longUnit(*this)
	, lirChange(*this)
	, sGUnit(*this)
	, udCounterCtrl(*AdxUdCounterCtrlCreate())
	, cntrEventHandler(*this)
{
	h = CreateEvent(NULL, TRUE, FALSE, NULL);
}

Lir::~Lir(void)
{
}

void Lir::Init(Advantech1730 *_dev, Advantech1730 *_dev2)
{
	dev = _dev;
	dev2 = _dev2;
	unsigned t = ItemIni::Get(Lir_inputs, L"crossUnit_bitStart", 0, fileName);
	crossUnit.bitStart = 1 << t;
	t = ItemIni::Get(Lir_inputs, L"crossUnit_bitStop", 1, fileName);
	crossUnit.bitStop = 1 << t;

	t = ItemIni::Get(Lir_inputs, L"thicknessUnit_bitStart", 4, fileName);
	thicknessUnit.bitStart = 1 << t;
	t = ItemIni::Get(Lir_inputs, L"thicknessUnit_bitStop", 5, fileName);
	thicknessUnit.bitStop = 1 << t;

	t = ItemIni::Get(Lir_inputs, L"longUnit_bitStart", 8, fileName);
	longUnit.bitStart = 1 << t;
	t = ItemIni::Get(Lir_inputs, L"longUnit_bitStop", 9, fileName);
	longUnit.bitStop = 1 << t;

	t = ItemIni::Get(Lir_inputs, L"LirChange_bit", 15, fileName);
	lirChange.bit = 1 << t;

//	6=ГП Вход
//7=ГП Выход
	t = ItemIni::Get(Lir_inputs, L"SGUnit_bit", 7, fileName);
	sGUnit.bit = 1 << t;

	samplesPerZoneParam[0] = ItemIni::Get(Lir_inputs, L"samplesPerZoneParam0", 180, fileName);
	samplesPerZoneParam[1] = ItemIni::Get(Lir_inputs, L"samplesPerZoneParam1", 180, fileName);


	wchar_t buf[128] = L"PCI-1784,BID#0";
	wchar_t *pBuf;
	pBuf = ItemIni::Get(Lir_description, L"Advantech1784", buf, fileName);
  #ifndef NO_1784
	ErrorCode errorCode = udCounterCtrl.setSelectedDevice(DeviceInformation(pBuf));
	if (BioFailed(errorCode))throw(Exception("A1730::A1784U: не смогли открыть плату Advantech1784U"));
	udCounterCtrl.addUdCntrEventListener(cntrEventHandler);
 #endif
	//запуск потока чтобы не нагружать обработчик платы 1784
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)__Run__, this, 0, NULL);
}

void Lir::Destroy()
{
	TerminateThread(hThread, 0);
}

void Lir::ChangeSamplesLir(int l0, int l1)
{
   samplesPerZoneParam[0] = l0;
   ItemIni::Set(Lir_inputs, L"samplesPerZoneParam0", l0, fileName);
   samplesPerZoneParam[1] = l1;
   ItemIni::Set(Lir_inputs, L"samplesPerZoneParam1", l1, fileName);
}

void Lir::GetSamplesLir(int &l0, int &l1)
{
   l0 = ItemIni::Get(Lir_inputs, L"samplesPerZoneParam0", 180, fileName);
   l1 = ItemIni::Get(Lir_inputs, L"samplesPerZoneParam1", 180, fileName);
}

void Lir::StartLir(int n)
{
  samplesPerZone = samplesPerZoneParam[n];
  if(n == udCounterCtrl.getChannel()) return;
//отключение одного лира
	udCounterCtrl.setChannel((n + 1) % 2);
	udCounterCtrl.setEnabled(false);

//подключение другого лира(смотри документацию на 1784)
	udCounterCtrl.setChannel(n);
	udCounterCtrl.setCountingType(TwoPulse);//AbPhaseX1);
	udCounterCtrl.CompareSetInterval(0, numberPulsesPerInterrupt, 100000000);
	udCounterCtrl.setEnabled(true);
}

void Lir::Start(bool isThick_, bool isLong_)
{
	isThick = isThick_;
	isLong = isLong_;
	crossUnit.Start();
	if(isThick)thicknessUnit.Start();
	if(isLong)longUnit.Start();
	lirChange.Start();
	sGUnit.Start();
	StartLir(0);
}

void Lir::Run()
{
	while(true)
	{
		WaitForSingleObject(h, INFINITE);
		inputBits = dev->Read();
		inputBits2 = dev2->Read();
		(crossUnit.*crossUnit.ptr)();
		(thicknessUnit.*thicknessUnit.ptr)();
		(longUnit.*longUnit.ptr)();
		(lirChange.*lirChange.ptr)();
		(sGUnit.*sGUnit.ptr)();
		ResetEvent(h);
	}
}

void Lir::__Run__(Lir *lir)
{
	lir->Run();
}

void Lir_Prepare(unsigned start, unsigned stop, unsigned (&time)[100], int &len)
{
	unsigned timeUnit = stop - start; //время прохождения через модуль
	timeUnit /= 2;
	unsigned zoneTime = time[1] - time[0]; // прохождение 1 зоны

	int zonesUnit = timeUnit / zoneTime; //
	int remZone = timeUnit % zoneTime;

	unsigned tmpTime[100];
	//
	unsigned startTime = start + timeUnit; //время захода трубы в зону измерения
	tmpTime[0] = startTime;
	for(int i = 0; i < zonesUnit; ++i)
	{
		tmpTime[i + 1] = startTime + (i + 1) * zoneTime;
	}

	for(int i = 0, j = zonesUnit; i < len; ++i, ++j)
	{
	   tmpTime[j] = time[i] - remZone;
	}
	len += zonesUnit;
	memcpy(time, tmpTime, sizeof(time));
}

void Lir::Prepare()
{
   Lir_Prepare(
	 crossUnit.bitStartUp
	 , crossUnit.bitStopUp
	 , crossUnit.zonesTime
	 , crossUnit.zonesTimeOffset
	 );
	 /*
	 for(int i = 1; i < crossUnit.zonesTimeOffset; ++i)
	 {
		 dprint("crossUnit %d %d %d %d\n", i
		 , crossUnit.zonesTime[i] - crossUnit.zonesTime[i - 1]
		 , crossUnit.zonesTime[i]
		 , crossUnit.zonesOffs[i] - crossUnit.zonesOffs[i - 1]
		 );
	 }
	 */

   if(isThick)
   {
   Lir_Prepare(
	 thicknessUnit.bitStartUp
	 , thicknessUnit.bitStopUp
	 , thicknessUnit.zonesTime
	 , thicknessUnit.zonesTimeOffset
	 );
	 /*
	 for(int i = 1; i < crossUnit.zonesTimeOffset; ++i)
	 {
		 dprint("thicknessUnit %d %d %d %d\n", i
		   , thicknessUnit.zonesTime[i] - thicknessUnit.zonesTime[i - 1]
		   , thicknessUnit.zonesTime[i]
		   , thicknessUnit.zonesOffs[i] - thicknessUnit.zonesOffs[i - 1]
		   );
	 }
	 */
   }

   if(isLong)
   {
	Lir_Prepare(
	 longUnit.bitStartUp
	 , longUnit.bitStopUp
	 , longUnit.zonesTime
	 , longUnit.zonesTimeOffset
	 );

/*
	 for(int i = 1; i < longUnit.zonesTimeOffset; ++i)
	 {
		 dprint("longUnit %d %d %d %d\n", i
		 , longUnit.zonesTime[i] - longUnit.zonesTime[i - 1]
		 , longUnit.zonesTime[i]
		 , longUnit.zonesOffs[i] - longUnit.zonesOffs[i - 1]
		 );
	 }
	 */
   }
}

Lir &Lir::Instance(){static Lir x; return x;}
//----------------------------------------------------------------------------------------------------------
CrossUnit::CrossUnit(Lir &o)
	: owner(o)
	, ptr(&CrossUnit::Noop)
{}
void CrossUnit::Start()
{
	onceUp = true;
	ptr = &CrossUnit::StartUp;
	zonesTimeOffset = 0;
}

void CrossUnit::StartUp()
{
	if(owner.inputBits & bitStart)
	{
		bitStartUp = timeGetTime();
		ptr = &CrossUnit::StopUp;
	}
}
void CrossUnit::StopUp()
{
	if(owner.inputBits & bitStop)
	{
	   lirOffsetUp = owner.tabIndex;
	   bitStopUp = timeGetTime();
	   ptr = &CrossUnit::StartDown;
	   startOffset = owner.tabIndex;
	}
}
void CrossUnit::StartDown()
{
	if(owner.inputBits & bitStart)
	{
		int t = owner.tabIndex - startOffset;
		if(t < 0) startOffset = owner.tabIndex;
		int tt = t / owner.samplesPerZone;
		tt *= owner.samplesPerZone;
		bool bit = t - tt <= owner.samplesPerZone / 2;
		if(onceUp && bit)
		{
			onceUp = false;
			zonesTime[zonesTimeOffset] = timeGetTime();
			zonesOffs[zonesTimeOffset] = t;
			++zonesTimeOffset;
		}
		if(!bit) onceUp = true;
	}
	else
	{
		bitStartDown = timeGetTime();
		ptr = &CrossUnit::StopDown;
	}
}
void CrossUnit::StopDown()
{
	if(owner.inputBits & bitStop)
	{
		int t = owner.tabIndex - startOffset;
		if(t < 0) startOffset = owner.tabIndex;
		int tt = t / owner.samplesPerZone;
		tt *= owner.samplesPerZone;
		bool bit = t - tt <= owner.samplesPerZone / 2;
		if(onceUp && bit)
		{
			onceUp = false;
			zonesTime[zonesTimeOffset] = timeGetTime();
			zonesOffs[zonesTimeOffset] = t;
			++zonesTimeOffset;
		}
		if(!bit) onceUp = true;
	}
	else
	{
		lirOffsetDown = owner.tabIndex;
		bitStartDown = timeGetTime();
		ptr = &CrossUnit::Noop;
//тк срабатывание лира на певом датчике не корректно - сэмитируем
		static const int widthModule = 630; //растояние между датчиками
		int t = int((double)(zonesTime[1] - zonesTime[0]) * widthModule / Lir::zoneLength);
		bitStartUp = bitStopUp - t;
	}
}

void CrossUnit::Noop()
{
}
//-------------------------------------------------------------------------
ThicknessUnit::ThicknessUnit(Lir &o)
	: owner(o)
	, ptr(&ThicknessUnit::Noop)
{}

void ThicknessUnit::Start()
{
	onceUp = true;
	ptr = &ThicknessUnit::StartUp;
	zonesTimeOffset = 0;
}

void ThicknessUnit::StartUp()
{
	if(owner.inputBits & bitStart)
	{
		bitStartUp = timeGetTime();
		ptr = &ThicknessUnit::StopUp;
	}
}
void ThicknessUnit::StopUp()
{
	if(owner.inputBits & bitStop)
	{
	   bitStopUp = timeGetTime();
	   ptr = &ThicknessUnit::StartDown;
	   startOffset = owner.tabIndex;
	}
}
void ThicknessUnit::StartDown()
{
	if(owner.inputBits & bitStart)
	{
		int t = owner.tabIndex - startOffset;
		if(t < 0) startOffset = owner.tabIndex;
		int tt = t / owner.samplesPerZone;
		tt *= owner.samplesPerZone;
		bool bit = t - tt <= owner.samplesPerZone / 2;
		if(onceUp && bit)
		{
			onceUp = false;
			zonesTime[zonesTimeOffset] = timeGetTime();
			zonesOffs[zonesTimeOffset] = t;
			++zonesTimeOffset;
		}
		if(!bit) onceUp = true;
	}
	else
	{
		bitStartDown = timeGetTime();
		ptr = &ThicknessUnit::StopDown;
	}
}
void ThicknessUnit::StopDown()
{
	if(owner.inputBits & bitStop)
	{
		int t = owner.tabIndex - startOffset;
		if(t < 0) startOffset = owner.tabIndex;
		int tt = t / owner.samplesPerZone;
		tt *= owner.samplesPerZone;
		bool bit = t - tt <= owner.samplesPerZone / 2;
		if(onceUp && bit)
		{
			onceUp = false;
			zonesTime[zonesTimeOffset] = timeGetTime();
			zonesOffs[zonesTimeOffset] = t;
			++zonesTimeOffset;
		}
		if(!bit) onceUp = true;
	}
	else
	{
		bitStartDown = timeGetTime();
		ptr = &ThicknessUnit::Noop;
	}
}

void ThicknessUnit::Noop()
{
}
//-----------------------------------------------------------------------------------
LongUnit::LongUnit(Lir &o)
	: owner(o)
	, ptr(&LongUnit::Noop)
{}

void LongUnit::Start()
{
	onceUp = true;
	ptr = &LongUnit::StartUp;
	zonesTimeOffset = 0;
}

void LongUnit::StartUp()
{
	if(owner.inputBits & bitStart)
	{
		bitStartUp = timeGetTime();
		ptr = &LongUnit::StopUp;
	}
}
void LongUnit::StopUp()
{
	if(owner.inputBits & bitStop)
	{
	   bitStopUp = timeGetTime();
	   ptr = &LongUnit::StartDown;
	   startOffset = owner.tabIndex;
	}
}
void LongUnit::StartDown()
{
	if(owner.inputBits & bitStart)
	{
		int t = owner.tabIndex - startOffset;
		if(t < 0) startOffset = owner.tabIndex;
		int tt = t / owner.samplesPerZone;
		tt *= owner.samplesPerZone;
		bool bit = t - tt <= owner.samplesPerZone / 2;
		if(onceUp && bit)
		{
			onceUp = false;
			zonesTime[zonesTimeOffset] = timeGetTime();
			zonesOffs[zonesTimeOffset] = t;
			++zonesTimeOffset;
		}
		if(!bit) onceUp = true;
	}
	else
	{
		bitStartDown = timeGetTime();
		ptr = &LongUnit::StopDown;
	}
}
void LongUnit::StopDown()
{
	if(owner.inputBits & bitStop)
	{
		int t = owner.tabIndex - startOffset;
		if(t < 0) startOffset = owner.tabIndex;
		int tt = t / owner.samplesPerZone;
		tt *= owner.samplesPerZone;
		bool bit = t - tt <= owner.samplesPerZone / 2;
		if(onceUp && bit)
		{
			onceUp = false;
			zonesTime[zonesTimeOffset] = timeGetTime();
			zonesOffs[zonesTimeOffset] = t;
			++zonesTimeOffset;
		}
		if(!bit) onceUp = true;
	}
	else
	{
		bitStartDown = timeGetTime();
		ptr = &LongUnit::Noop;
	}
}
void LongUnit::Noop()
{
}
//--------------------------------------------------------------------------------
LirChange::LirChange(Lir &o)
: owner(o)
, ptr(&LirChange::Noop)
, samplesPerZone0(0)
, samplesPerZone1(0)
{

}
void LirChange::Start()
{
	ptr = &LirChange::StartUp;
}
void LirChange::StartUp()
{
   if(owner.inputBits & bit)
   {
	   lirOffsetUp = owner.tabIndex;
	   delay = owner.tabIndex + 2 * owner.samplesPerZone;
	   ptr = &LirChange::Delay;
   }
}
void LirChange::Delay()
{
	 if(owner.tabIndex > delay)
	 {
		  owner.StartLir(1);
		  owner.cntrEventHandler.lastCounter = owner.udCounterCtrl.getValue();
		  ptr = &LirChange::StartDown;
	 }
}
//lirOffsetDown = owner.tabIndex;
void LirChange::StartDown()
{
  if(0 == (owner.inputBits & bit))
  {
	 lirOffsetDown = owner.tabIndex;
	 ptr = &LirChange::Noop;
	 samplesPerZone0 = (double)Lir::zoneLength
	   * (lirOffsetUp - owner.crossUnit.lirOffsetUp)/Lir::distantionSQ9_SQ26;
	 samplesPerZone1 = (double)Lir::zoneLength
	   * (lirOffsetDown - owner.crossUnit.lirOffsetDown)/Lir::distantionSQ9_SQ26;
  }
}
void LirChange::Noop()
{

}
//-------------------------------------------------------------------------
SGUnit::SGUnit(Lir &o)
: owner(o)
, ptr(&SGUnit::Noop)
, bit2(o.longUnit.bitStop)
{

}
void SGUnit::Start()
{
	ptr = &SGUnit::StartUp;
}
void SGUnit::StartUp()
{
   if(owner.inputBits2 & bit)
   {
	   ptr = &SGUnit::StartDown;
	   bitTimeUp = timeGetTime();
	   unitEmpty = false;
   }
}

void SGUnit::StartDown()
{
  if(0 == (owner.inputBits & bit2))
  {
	 ptr = &SGUnit::Noop;
	 bitTimeDown = timeGetTime();
	 unitEmpty = true;
  }
}
void SGUnit::Noop()
{

}

