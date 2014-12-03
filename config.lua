HOST = "127.0.0.1"              -- listen for, if set HOST to "", frontend will accept any addr
PORT = 1077                     -- frontend listen port
MAX_PKG_SIZE = 4096             -- tcp pkg size transfered between backend and frontend
BACKEND_MS = "backend.exe"       -- backend program name, in windows
BACKEND_UNIX = "backend"         -- backend program name, in unix

function get_os_type()
   dir_sep = string.sub(package.config,1,1)
   if dir_sep == '\\' then
      return "windows"
   else
      return "unix"
   end
end

if get_os_type() == "windows" then
   BACKEND = BACKEND_MS
else
   BACKEND = BACKEND_UNIX
end
