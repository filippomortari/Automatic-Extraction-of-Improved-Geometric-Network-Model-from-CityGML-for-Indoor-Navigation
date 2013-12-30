#pragma once
class EnvironmentParameters
{
public:
	static EnvironmentParameters* getInstance();
	double lOffset;
	double samplingRate;
	double precision;
private:
	static EnvironmentParameters* instance;
	EnvironmentParameters(void);

};

