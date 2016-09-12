/** \file  read_conf.h
 *  \brief Read the configure file to get the data buffer and buffer length.
 *  \date  2016-07-26
 *  \email da.li@cripac.ia.ac.cn
 */
#ifndef _READ_CONF_H_
#define _READ_CONF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;
//#include "type_def.h"

/** \fn readConfFile
 *  \brief Read configure file.
 *  \param[IN] path - file path.
 *  \param[OUT] buffer - file data.
 *  \return buffer length.
 */
size_t readConfFile(const string& path, char* buffer);

#endif  // _READ_CONF_H_
