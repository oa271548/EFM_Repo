#include "stdafx.h"
#include "chart.h"

// Data adding is quite fast so multiply the expected sample size with 2.1
// to ensure the correct time range
// The value 2.1 is selected by practical experiment with the range of 5 minutes
// 24 hours = 60 * 60 * 24 = 86400 seconds
// 0.1 of 24 hours = 86400 * 0.1 = 8640 --> round up to 10000 seconds
static const int SampleSize = 86400 * 2 + 10000;

// The maximum zoom in is 3 seconds.
static const int ZoomInLimit = 3;

EFMChart::EFMChart()
{
	m_chart = new XYChart(300, 300);

	// Allocate memory for the data series and initialize to Chart::NoValue
	m_timeStamps.resize(SampleSize, Chart::NoValue * (-1.0));
	m_dataSeries.resize(SampleSize, Chart::NoValue);

	// Add first data item to force the grid line the be showing
	SYSTEMTIME st;
	GetLocalTime(&st);
	double now = Chart::chartTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond)
		+ st.wMilliseconds / 1000.0;

	// Enable mouse wheel zooming by setting the zoom ratio to 1.4 per wheel event
	setMouseWheelZoomRatio(1.4);
	setMouseUsage(Chart::MouseUsageScroll);
	setZoomDirection(2);
	setFullRange("y", -20, 20);
	setFullRange("x", now - 24.0 * 60 * 60, now);
}

EFMChart::~EFMChart()
{
	delete m_chart;
	m_chart = NULL;

	if (m_vpTimestamps != NULL) delete[] m_vpTimestamps;
	if (m_vpData != NULL) delete[] m_vpData;
}

void EFMChart::drawChart()
{
	// Get the start date and end date that are visible on the chart.
	double viewPortStartDate = getValueAtViewPort("x", getViewPortLeft());
	double viewPortEndDate = getValueAtViewPort("x", getViewPortRight());
	double viewPortValMax = getValueAtViewPort("y", getViewPortTop()) * -1.0;
	double viewPortValMin = getValueAtViewPort("y", getViewPortBottom()) * -1.0;

	// Extract the part of the data arrays that are visible.
	DoubleArray viewPortTimeStamps;
	DoubleArray viewPortDataSeries;

	if (!m_firstDataAdd)
	{
		DoubleArray da_timeStamps = vectorToArray(m_timeStamps);
		int startIndex = (int)floor(Chart::bSearch(da_timeStamps, viewPortStartDate));
		int endIndex = (int)ceil(Chart::bSearch(da_timeStamps, viewPortEndDate));
		int noOfPoints = endIndex - startIndex + 1;
		// Extract the visible data
		if (m_vpTimestamps != NULL) delete [] m_vpTimestamps;
		if (m_vpData != NULL) delete [] m_vpData;
		m_vpTimestamps = new double[noOfPoints];
		m_vpData = new double[noOfPoints];

		memcpy(m_vpTimestamps, &m_timeStamps[startIndex], noOfPoints * sizeof(double));
		memcpy(m_vpData, &m_dataSeries[startIndex], noOfPoints * sizeof(double));
		viewPortTimeStamps = DoubleArray(m_vpTimestamps, noOfPoints);
		viewPortDataSeries = DoubleArray(m_vpData, noOfPoints);
	}
	else
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		double now = Chart::chartTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond)
			+ st.wMilliseconds / 1000.0;
		viewPortStartDate = now - m_initialFullRange;
		viewPortEndDate = now;
	}

	if (m_requestNewXSpan) {
		m_requestNewXSpan = false;
		setViewPortLeft(0);
		setViewPortWidth(1);
	}

	delete m_chart;
	m_chart = new XYChart(m_width, m_height);
	m_chart->setBackground(0x000000);

	// Set the plotarea. Use black (0x000000) background.
	// Enable both horizontal and vertical grids by setting their colors to 
	// grey (0x404040). Set clipping mode to clip the data lines to the plot area.
	m_chart->setPlotArea(50, 30, m_width - 80, m_height - 80, 0x000000, -1, -1, 0x404040, 0x404040);
	m_chart->setClipping();

	m_chart->addTitle(m_caption.GetString(), "Arial", 11, m_captionColor)->setAlignment(0);

	// Configure the x-axis to auto-scale with at least 75 pixels between major tick and 
	// 15  pixels between minor ticks. This shows more minor grid lines on the chart.
	m_chart->xAxis()->setTickDensity(66, 10);
	m_chart->xAxis2()->setTickDensity(66, 10);
	m_chart->getPlotArea()->setGridWidth(1, 1, 0, 0);

	// Increase size of labels
	m_chart->xAxis()->setLabelStyle("Arial", 10);
	m_chart->yAxis()->setLabelStyle("Arial", 10);

	// Set the axes colors
	m_chart->xAxis()->setColors(0x404040, 0xdddddd, -1, 0xdddddd);
	m_chart->yAxis()->setColors(0x404040, 0xdddddd, -1, 0xdddddd);
	m_chart->xAxis2()->setColors(0x404040, 0x000000, -1, 0xdddddd);
	m_chart->yAxis2()->setColors(0x404040, 0x000000, -1, 0xdddddd);

	m_chart->yAxis()->setTickDensity(40, 20);
	if (m_enableAutoScale)
	{
		m_chart->xAxis()->setAutoScale();
		m_chart->setAxisAtOrigin(2, 4);
	}
	else
	{
		m_chart->yAxis()->setLinearScale(viewPortValMin, viewPortValMax);
	}
	m_chart->yAxis()->setRounding(false, false);

	m_chart->syncYAxis();
	

	// Now we add the data to the chart. 
	m_chart->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);
	m_chart->xAxis2()->setDateScale(viewPortStartDate, viewPortEndDate);

	// If all ticks are minute algined, then we use "hh:nn" as the label format.
	m_chart->xAxis()->setFormatCondition("align", 60);
	m_chart->xAxis()->setLabelFormat("{value|hh:nn}");

	// If all other cases, we use "hh:nn:ss" as the label format.
	m_chart->xAxis()->setFormatCondition("else");
	m_chart->xAxis()->setLabelFormat("{value|hh:nn:ss}");

	// We make sure the tick increment must be at least 1 second.
	m_chart->xAxis()->setMinTickInc(1);

	if (m_BShowColorLineHFA)
	{
		Mark* yMarkHF = m_chart->yAxis()->addMark(m_dSetPointHFA, 0xffff00);
		yMarkHF = m_chart->yAxis()->addMark(-m_dSetPointHFA, 0xffff00);
		yMarkHF->setLineWidth(1);
		yMarkHF->setAlignment(Chart::TopCenter);
	}

	if (m_BShowColorLineVHFA)
	{
		Mark* yMarkVHF = m_chart->yAxis()->addMark(m_dSetpointVHFA, 0xff0000);
		yMarkVHF = m_chart->yAxis()->addMark(-m_dSetpointVHFA, 0xff0000);
		yMarkVHF->setLineWidth(1);
		yMarkVHF->setAlignment(Chart::TopCenter);
	}

	// Create a line layer to plot the lines
	LineLayer* layer = m_chart->addLineLayer();
	layer->setXData(viewPortTimeStamps);
	layer->addDataSet(viewPortDataSeries, m_dataLineColor, "Data");
	layer->setLineWidth(2);
	
	// Output the chart
	this->setChart(m_chart);
}

void EFMChart::setSize(int w, int h)
{
	m_width = w;
	m_height = h;
	drawChart();
}

void EFMChart::setCaption(CString caption)
{
	m_caption = caption;
}

CString EFMChart::getCaption()
{
	return m_caption;
}

void EFMChart::setCaptionColor(int color)
{
	m_captionColor = color;
}

void EFMChart::setDataLineColor(int color)
{
	m_dataLineColor = color;
}


void EFMChart::setHFASetPoint(double dSetPointHFA)
{
	m_dSetPointHFA = dSetPointHFA;
}
void EFMChart::setShowColorLineHFA(BOOL bShowColorLineHFA)
{
	m_BShowColorLineHFA = bShowColorLineHFA;
}

void EFMChart::setVHFASetPoint(double dVHFASetpoint)
{
	m_dSetpointVHFA = dVHFASetpoint;
}

void EFMChart::setShowColorLineVHFA(BOOL bEnableVHFA)
{
	m_BShowColorLineVHFA = bEnableVHFA;
}

void EFMChart::addXYData(double data[10][2], int length)
{
	if (m_firstDataAdd)
	{
		m_firstDataAdd = FALSE;
	}

	int numOfElementToMove = SampleSize - length;
	int moveSize = sizeof(double) * numOfElementToMove;
	memmove(&m_timeStamps[0], &m_timeStamps[length], moveSize);
	memmove(&m_dataSeries[0], &m_dataSeries[length], moveSize);

	int index = m_timeStamps.size() - length;
	for (int j = 0; j < length; ++j)
	{
		m_timeStamps[index] = data[j][0];
		m_dataSeries[index] = data[j][1];
		index++;
	}

	m_totalAdded += length;
	if (m_totalAdded > SampleSize) m_totalAdded = SampleSize;

	if (!m_firstDataAdd)
	{
		updateChart();
	}
}

void EFMChart::setXSpan(double value)
{
	if (value == 1.0 / 24.0 / 60.0 * 5.5)
	{
		m_initialFullRange = 5 * 60;
	}
	if (value == 1.0 / 24.0 / 60.0 * 15.0)
	{
		m_initialFullRange = 15 * 60;
	}
	if (value == 1.0 / 24.0 / 60.0 * 30.0)
	{
		m_initialFullRange = 30 * 60;
	}
	if (value == 1.0 / 24.0)
	{
		m_initialFullRange = 60 * 60;
	}
	if (value == 1.0 / 24.0 * 2.0)
	{
		m_initialFullRange = 2 * 60 * 60;
	}
	if (value == 1.0 / 24.0 * 4.0)
	{
		m_initialFullRange = 4 * 60 * 60;
	}
	if (value == 1.0 / 24.0 * 8.0)
	{
		m_initialFullRange = 8 * 60 * 60;
	}
	if (value == 1.0)
	{
		m_initialFullRange = 24 * 60 * 60;
	}

	m_requestNewXSpan = true;
	if (!m_firstDataAdd)
	{
		updateChart();
	}
	updateViewPort(true, false);
}

void EFMChart::setYSpan(double min, double max)
{
	disableAutoScale();

	setFullRange("y", min, max);
	setViewPortTop(0);
	setViewPortHeight(1);
	updateViewPort(true, false);
}

void EFMChart::enableAutoScale()
{
	m_enableAutoScale = true;
}

void EFMChart::disableAutoScale()
{
	m_enableAutoScale = false;
}

//
// Convert std::vector to a DoubleArray
//
DoubleArray EFMChart::vectorToArray(const std::vector<double>& v, int startIndex, int length)
{
	if ((length < 0) || (length + startIndex > (int)v.size()))
		length = ((int)v.size()) - startIndex;
	return (length <= 0) ? DoubleArray() : DoubleArray(&(v[startIndex]), length);
}

void EFMChart::updateChart()
{
	//
	// As we added more data, we may need to update the full range of the viewport. 
	//

	double startDate = m_timeStamps[SampleSize - m_totalAdded];
	double endDate = m_timeStamps[SampleSize - 1];

	// Use the initialFullRange
	startDate = endDate - m_initialFullRange;

	// Update the full range to reflect the actual duration of the data. In this case, 
	// if the view port is viewing the latest data, we will scroll the view port as new
	// data are added. If the view port is viewing historical data, we would keep the 
	// axis scale unchanged to keep the chart stable.
	int updateType = Chart::ScrollWithMax;
	if (getViewPortLeft() + getViewPortWidth() < 0.999) updateType = Chart::KeepVisibleRange;
	bool scaleHasChanged = updateFullRangeH("x", startDate, endDate, updateType);

	// Set the zoom in limit as a ratio to the full range
	setZoomInWidthLimit(ZoomInLimit / (getValueAtViewPort("x", 1) - getValueAtViewPort("x", 0)));
}

CLightningDistanceChart::CLightningDistanceChart()
{
	m_chart = new XYChart(300, 300);

	// Allocate memory for the data series and initialize to Chart::NoValue
	m_timeStamps.resize(SampleSize, Chart::NoValue * (-1.0));
	m_dataSeries.resize(SampleSize, Chart::NoValue);
	// Add first data item to force the grid line the be showing
	SYSTEMTIME st;
	GetLocalTime(&st);
	double now = Chart::chartTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond)
		+ st.wMilliseconds / 1000.0;

	// Enable mouse wheel zooming by setting the zoom ratio to 1.4 per wheel event
	setMouseWheelZoomRatio(1.4);
	setMouseUsage(Chart::MouseUsageScroll);
	setZoomDirection(2);
	setFullRange("y", 0.0, 36.0);
	setFullRange("x", now - 24.0 * 60 * 60, now);
}

CLightningDistanceChart::~CLightningDistanceChart()
{
	delete m_chart;
	m_chart = NULL;

	if (m_vpTimestamps != NULL) delete[] m_vpTimestamps;
	if (m_vpData != NULL) delete[] m_vpData;
}


void CLightningDistanceChart::drawChart()
{
	// Get the start date and end date that are visible on the chart.
	double viewPortStartDate = getValueAtViewPort("x", getViewPortLeft());
	double viewPortEndDate = getValueAtViewPort("x", getViewPortRight());
	double viewPortValMax =  24.0;
	double viewPortValMin =  0.0;

	// Extract the part of the data arrays that are visible.
	DoubleArray viewPortTimeStamps;
	DoubleArray viewPortDataSeries;
	if (!m_firstDataAdd)
	{
		DoubleArray da_timeStamps = vectorToArray(m_timeStamps);
		int startIndex = (int)floor(Chart::bSearch(da_timeStamps, viewPortStartDate));
		int endIndex = (int)ceil(Chart::bSearch(da_timeStamps, viewPortEndDate));
		int noOfPoints = endIndex - startIndex + 1;
		// Extract the visible data
		if (m_vpTimestamps != NULL) delete[] m_vpTimestamps;
		m_vpTimestamps = new double[noOfPoints];
		m_vpData = new double[noOfPoints];

		memcpy(m_vpTimestamps, &m_timeStamps[startIndex], noOfPoints * sizeof(double));
		memcpy(m_vpData, &m_dataSeries[startIndex], noOfPoints * sizeof(double));
		viewPortTimeStamps = DoubleArray(m_vpTimestamps, noOfPoints);
		viewPortDataSeries = DoubleArray(m_vpData, noOfPoints);
	}
	else
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		double now = Chart::chartTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond)
			+ st.wMilliseconds / 1000.0;
		viewPortStartDate = now - m_initialFullRange;
		viewPortEndDate = now;
	}

	if (m_requestNewXSpan) {
		m_requestNewXSpan = false;
		setViewPortLeft(0);
		setViewPortWidth(1);
	}

	delete m_chart;
	m_chart = new XYChart(m_width, m_height);
	m_chart->setBackground(0x000000);

	// Set the plotarea. Use black (0x000000) background.
	// Enable both horizontal and vertical grids by setting their colors to 
	// grey (0x404040). Set clipping mode to clip the data lines to the plot area.
	m_chart->setPlotArea(50, 30, m_width - 80, m_height - 80, 0x000000, -1, -1, 0x404040, 0x404040);
	m_chart->setClipping();

	m_chart->addTitle(m_caption.GetString(), "Arial", 11, m_captionColor)->setAlignment(0);

	// Configure the x-axis to auto-scale with at least 75 pixels between major tick and 
	// 15  pixels between minor ticks. This shows more minor grid lines on the chart.
	m_chart->xAxis()->setTickDensity(66, 10);
	m_chart->xAxis2()->setTickDensity(66, 10);
	m_chart->getPlotArea()->setGridWidth(1, 1, 0, 0);

	// Increase size of labels
	m_chart->yAxis()->setTitle("Lightnig Distance (miles)");
	m_chart->xAxis()->setLabelStyle("Arial", 10);
	m_chart->yAxis()->setLabelStyle("Arial", 10);

	// Set the axes colors
	m_chart->xAxis()->setColors(0x404040, 0xdddddd, -1, 0xdddddd);
	m_chart->yAxis()->setColors(0x404040, 0xdddddd, -1, 0xdddddd);
	m_chart->xAxis2()->setColors(0x404040, 0x000000, -1, 0xdddddd);
	m_chart->yAxis2()->setColors(0x404040, 0x000000, -1, 0xdddddd);

	m_chart->yAxis()->setTickDensity(40, 20);
	m_chart->yAxis()->setLinearScale(viewPortValMin, viewPortValMax);
	m_chart->yAxis()->setRounding(false, false);

	m_chart->syncYAxis();


	// Now we add the data to the chart. 
	m_chart->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);
	m_chart->xAxis2()->setDateScale(viewPortStartDate, viewPortEndDate);

	// If all ticks are minute algined, then we use "hh:nn" as the label format.
	m_chart->xAxis()->setFormatCondition("align", 60);
	m_chart->xAxis()->setLabelFormat("{value|hh:nn}");

	// If all other cases, we use "hh:nn:ss" as the label format.
	m_chart->xAxis()->setFormatCondition("else");
	m_chart->xAxis()->setLabelFormat("{value|hh:nn:ss}");

	// We make sure the tick increment must be at least 1 second.
	m_chart->xAxis()->setMinTickInc(1);

	// Create a line layer to plot the lines
	LineLayer* layer = m_chart->addLineLayer();
	layer->setXData(viewPortTimeStamps);
	layer->addDataSet(viewPortDataSeries, m_dataLineColor, "Data");
	layer->setLineWidth(2);

	// Output the chart
	this->setChart(m_chart);
}

void CLightningDistanceChart::setXSpan(double value)
{
	if (value == 1.0 / 24.0 / 60.0 * 5.5)
	{
		m_initialFullRange = 5 * 60;
	}
	if (value == 1.0 / 24.0 / 60.0 * 15.0)
	{
		m_initialFullRange = 15 * 60;
	}
	if (value == 1.0 / 24.0 / 60.0 * 30.0)
	{
		m_initialFullRange = 30 * 60;
	}
	if (value == 1.0 / 24.0)
	{
		m_initialFullRange = 60 * 60;
	}
	if (value == 1.0 / 24.0 * 2.0)
	{
		m_initialFullRange = 2 * 60 * 60;
	}
	if (value == 1.0 / 24.0 * 4.0)
	{
		m_initialFullRange = 4 * 60 * 60;
	}
	if (value == 1.0 / 24.0 * 8.0)
	{
		m_initialFullRange = 8 * 60 * 60;
	}
	if (value == 1.0)
	{
		m_initialFullRange = 24 * 60 * 60;
	}

	m_requestNewXSpan = true;
	if (!m_firstDataAdd)
	{
		updateChart();
	}
	updateViewPort(true, false);
}


void CLightningDistanceChart::setSize(int w, int h)
{
	m_width = w;
	m_height = h;
	drawChart();
}


void CLightningDistanceChart::setCaptionColor(int color)
{
	m_captionColor = color;
}


DoubleArray CLightningDistanceChart::vectorToArray(const std::vector<double>& v, int startIndex, int length)
{
	if ((length < 0) || (length + startIndex > (int)v.size()))
		length = ((int)v.size()) - startIndex;
	return (length <= 0) ? DoubleArray() : DoubleArray(&(v[startIndex]), length);
}


void CLightningDistanceChart::updateChart()
{
	//
	// As we added more data, we may need to update the full range of the viewport. 
	//

	double startDate = m_timeStamps[SampleSize - m_totalAdded];
	double endDate = m_timeStamps[SampleSize - 1];

	// Use the initialFullRange
	startDate = endDate - m_initialFullRange;

	// Update the full range to reflect the actual duration of the data. In this case, 
	// if the view port is viewing the latest data, we will scroll the view port as new
	// data are added. If the view port is viewing historical data, we would keep the 
	// axis scale unchanged to keep the chart stable.
	int updateType = Chart::ScrollWithMax;
	if (getViewPortLeft() + getViewPortWidth() < 0.999) updateType = Chart::KeepVisibleRange;
	bool scaleHasChanged = updateFullRangeH("x", startDate, endDate, updateType);

	// Set the zoom in limit as a ratio to the full range
	setZoomInWidthLimit(ZoomInLimit / (getValueAtViewPort("x", 1) - getValueAtViewPort("x", 0)));
}

void CLightningDistanceChart::setCaption(CString caption)
{
	m_caption = caption;
}


void CLightningDistanceChart::addXYData(double data[10][2], int length)
{
	if (m_firstDataAdd)
	{
		m_firstDataAdd = FALSE;
	}

	int numOfElementToMove = SampleSize - length;
	int moveSize = sizeof(double) * numOfElementToMove;
	memmove(&m_timeStamps[0], &m_timeStamps[length], moveSize);
	memmove(&m_dataSeries[0], &m_dataSeries[length], moveSize);

	int index = m_timeStamps.size() - length;
	for (int j = 0; j < length; ++j)
	{
		m_timeStamps[index] = data[j][0];
		m_dataSeries[index] = data[j][1];
		index++;
	}

	m_totalAdded += length;
	if (m_totalAdded > SampleSize) m_totalAdded = SampleSize;

	if (!m_firstDataAdd)
	{
		updateChart();
	}
}

void CLightningDistanceChart::setDataLineColor(int color)
{
	m_dataLineColor = color;
}