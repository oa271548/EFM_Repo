#pragma once

#ifndef _EFMChart_
#define _EFMChart_

#include "ChartViewer.h"

class EFMChart : public CChartViewer
{
public:
	EFMChart();
	~EFMChart();

	void drawChart();
	void setSize(int, int);
	void addXYData(double data[10][2], int length);
	void setXSpan(double);
	void setYSpan(double min, double max);
	void setCaption(CString);
	CString getCaption();
	void setCaptionColor(int);
	void setDataLineColor(int);
	void enableAutoScale();
	void disableAutoScale();
	void setHFASetPoint(double);
	void setShowColorLineHFA(BOOL);
	void setVHFASetPoint(double);
	void setShowColorLineVHFA(BOOL);

private:
	XYChart* m_chart;
	int m_width = 30;
	int m_height = 30;
	CString m_caption;
	int m_captionColor = 0xffffff;
	bool m_enableAutoScale = true;
	int m_dataLineColor = 0x00ff00;
	bool m_firstDataAdd = true;
	std::vector<double> m_timeStamps;	// The timestamps for the data series
	DoubleArray vectorToArray(const std::vector<double>& v, int startIndex = 0, int length = -1);
	std::vector<double> m_dataSeries;	// The values for the data series
	double* m_vpTimestamps = NULL;
	double* m_vpData = NULL;
	bool m_requestNewXSpan = false;
	int m_totalAdded = 0;
	int m_initialFullRange = 24 * 60 * 60;
	BOOL m_BShowColorLineHFA{FALSE};
	double m_dSetPointHFA{0.0};
	BOOL m_BShowColorLineVHFA{FALSE};
	double m_dSetpointVHFA{0.0};

	// Convert vector to DoubleArray
	
	void updateChart();
};

class CLightningDistanceChart : public CChartViewer
{
public:
	CLightningDistanceChart();
	~CLightningDistanceChart();
	void drawChart();
	void setSize(int, int);
	void setXSpan(double);
	void setCaptionColor(int);
	void setCaption(CString caption);
	void addXYData(double data[10][2], int length);
	void setDataLineColor(int);

private:
	XYChart* m_chart = nullptr;
	int m_width = 30;
	int m_height = 30;
	CString m_caption;
	int m_captionColor = 0xffffff;
	bool m_enableAutoScale = true;
	int m_dataLineColor = 0x00ff00;
	bool m_firstDataAdd = true;
	std::vector<double> m_dataSeries;	// The values for the data series
	double* m_vpTimestamps = NULL;
	double* m_vpData = NULL;
	bool m_requestNewXSpan = false;
	int m_totalAdded = 0;
	int m_initialFullRange = 24 * 60 * 60;
	std::vector<double> m_timeStamps;	// The timestamps for the data series
	DoubleArray vectorToArray(const std::vector<double>& v, int startIndex = 0, int length = -1);
	void updateChart();
};


#endif // !_EFMChart_
