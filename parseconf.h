#ifndef PARSECONF_H
#define PARSECONF_H

#include <QString>
#include <QMessageBox>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define CONFIG_FILE "config.lua"

int MaxPktSize(void);
void ParseConf(QString *host, int *port, QString *backend);

#endif // GETCONFIG_H
