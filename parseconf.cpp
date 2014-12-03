#include "parseconf.h"
#include <QDir>
#include <QFile>
#include <QApplication>

void ErrorExit(QString title, QString text)
{
    QMessageBox::critical(NULL, title, text);
    qApp->exit();
}

QString GetFullCfgPath()
{
    QDir dir;
    return dir.absolutePath() + QDir::separator() + QString::fromUtf8(CONFIG_FILE);
}

bool ConfExist()
{
    QFile f;

    f.setFileName(GetFullCfgPath());

    return f.exists();
}


void ParseConf(QString *host, int *port, QString *backend)
{
    bool isErr;
    const char *h, *b;
    QString errMsg;

    if(false == ConfExist()) {
        errMsg = "Can not open configure file '" + GetFullCfgPath() + "'";
        ErrorExit("Configure", errMsg);
    }

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    isErr = luaL_dofile(L, CONFIG_FILE);
    if(isErr == true) {
        errMsg = "Can not open configure file '" + GetFullCfgPath() + "'";
        ErrorExit("Configure", errMsg);
    }

    lua_getglobal(L, "HOST");
    lua_getglobal(L, "PORT");
    lua_getglobal(L, "BACKEND");

    if (!lua_isstring(L, -3)) {
        lua_close(L);
        ErrorExit("Configure", "'HOST' should be a string");
    }

    if (!lua_isnumber(L, -2)) {
        lua_close(L);
        ErrorExit("Configure", "'PORT' should be a number");
    }

    if (!lua_isstring(L, -1)) {
        lua_close(L);
        ErrorExit("Configure", "'BACKEND' should be a string");
    }

    h = lua_tostring(L, -3);
    *port = lua_tointeger(L, -2);
    b = lua_tostring(L, -1);

    *host = QString::fromStdString(h);
    *backend = QString::fromStdString(b);

    lua_close(L);
}

int MaxPktSize(void)
{
    int size;
    QString errMsg;

    if(false == ConfExist()) {
        errMsg = "Can not open configure file '" + GetFullCfgPath() + "'";
        ErrorExit("Configure", errMsg);
    }

    lua_State *L = luaL_newstate();
    luaL_dofile(L, CONFIG_FILE);

    lua_getglobal(L, "MAX_PKG_SIZE");

    if (!lua_isnumber(L, -1)) {
        lua_close(L);
        ErrorExit("Configure", "'MAX_PKG_SIZE' should be a string");
    }

    size = lua_tointeger(L, -1);

    lua_close(L);
    return size;
}
