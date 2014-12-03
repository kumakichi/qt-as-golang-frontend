#include "parseconf.h"
#include <QDir>

void ShowError(QString title, QString text)
{
    QMessageBox::critical(NULL, title, text);
}

void ParseConf(QString *host, int *port, QString *backend)
{
    bool isErr;
    const char *h, *b;
    QString errMsg, cur;
    QDir dir;

    cur = dir.absolutePath();

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    isErr = luaL_dofile(L, CONFIG_FILE);
    if(isErr == true) {
        errMsg = "Open configure file '" + cur + QDir::separator() + QString::fromUtf8(CONFIG_FILE) + "' failed";
        ShowError("Configure", errMsg);
    }

    lua_getglobal(L, "HOST");
    lua_getglobal(L, "PORT");
    lua_getglobal(L, "BACKEND");

    if (!lua_isstring(L, -3)) {
        lua_close(L);
        ShowError("Configure", "'HOST' should be a string");
    }

    if (!lua_isnumber(L, -2)) {
        lua_close(L);
        ShowError("Configure", "'PORT' should be a number");
    }

    if (!lua_isstring(L, -1)) {
        lua_close(L);
        ShowError("Configure", "'BACKEND' should be a string");
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

    lua_State *L = luaL_newstate();
    luaL_dofile(L, CONFIG_FILE);

    lua_getglobal(L, "MAX_PKG_SIZE");

    if (!lua_isnumber(L, -1))
        luaL_error(L, "'PORT' should be a number\n");

    size = lua_tointeger(L, -1);

    lua_close(L);
    return size;
}
