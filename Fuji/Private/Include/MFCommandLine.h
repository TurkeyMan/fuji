/**
 * @file MFCommandLine.h
 * @brief Provides a set of functions to parse the command line easilly.
 * @author Manu Evans
 * @defgroup CommandLine Command Line Parsing Functions
 * @{
 */

#if !defined(_MFCOMMANDLINE_H)
#define _MFCOMMANDLINE_H

/**
 * Get the command line buffer.
 * Gets the command line buffer.
 * @return Returns a pointer to the internal command line buffer which can be used for custom parsing.
 */
const char* MFCommandLine_GetCommandLineString();

/**
 * Get a bool argument from the command line.
 * Gets a bool argument from the command line.
 * @param pParamater Name of the paramater to fetch from the command line.
 * @param defaultValue Default value to use if paramater does not appear on the command line.
 * @return Returns the value of the paramater on the command line as a bool. If the paramater does not exist, \a defaultValue is returned.
 */
bool MFCommandLine_GetBool(const char *pParamater, bool defaultValue = false);

/**
 * Get an int argument from the command line.
 * Gets a int argument from the command line.
 * @param pParamater Name of the paramater to fetch from the command line.
 * @param defaultValue Default value to use if paramater does not appear on the command line.
 * @return Returns the value of the paramater on the command line as an int. If the paramater does not exist, \a defaultValue is returned.
 */
int MFCommandLine_GetInt(const char *pParamater, int defaultValue = 0);

/**
 * Get a float argument from the command line.
 * Gets a float argument from the command line.
 * @param pParamater Name of the paramater to fetch from the command line.
 * @param defaultValue Default value to use if paramater does not appear on the command line.
 * @return Returns the value of the paramater on the command line as a float. If the paramater does not exist, \a defaultValue is returned.
 */
float MFCommandLine_GetFloat(const char *pParamater, float defaultValue = 0.0f);

/**
 * Get a string argument from the command line.
 * Gets a string argument from the command line.
 * @param pParamater Name of the paramater to fetch from the command line.
 * @return Returns the value of the paramater on the command line as a string. If the paramater does not exist, NULL is returned.
 */
const char* MFCommandLine_GetString(const char *pParamater);

#endif

/** @} */
