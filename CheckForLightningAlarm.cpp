bool CEFMDlg::CheckForLightningAlarm(int iIndex, double dReading)
//
// Call with field mill number (0-3) and current field mill reading (-20.0 kV/m to +20.0 kV/m)
// Returns true if lightning detected, else returns false
{
	static double field  [MAX_NUMOFDATASOURCES][3]; // last 3 readings of field data
	static double slope  [MAX_NUMOFDATASOURCES][2]; // slope of prev field data
	static double slope2 [MAX_NUMOFDATASOURCES];    // slope of the slope

	// Age the readings
	field[iIndex][2] = field[iIndex][1];
	field[iIndex][1] = field[iIndex][0];
	field[iIndex][0] = dReading;

	// Calculate slope of the recent data
	slope[iIndex][1] = slope[iIndex][0];
	slope[iIndex][0] = field[iIndex][0]-field[iIndex][1];

	// Calculate slope of slope of the recent data
	slope2[iIndex] = slope[iIndex][0]-slope[iIndex][1];

	// Check magnitude of slope of slope data
//	TRACE("slope2[%d]: %f field[%d][1-2]: %f\n", iIndex, slope2[iIndex], iIndex, field[iIndex][1]-field[iIndex][2]);
	if (fabs(slope2[iIndex]) < config.m_dLASensitivity)
		return false; // leave, no slope changes here

	// Possible lightning: Check that we have a step change in field (not rain noise)
	if ((fabs(field[iIndex][1]-field[iIndex][2])) < config.m_dLASensitivity)
		return false; // leave, no step changes here

//	TRACE("**********LIGHTNING WAS DETECTED!***********\n");
	return true; // We Found Lightning! We passed all the tests.
}
