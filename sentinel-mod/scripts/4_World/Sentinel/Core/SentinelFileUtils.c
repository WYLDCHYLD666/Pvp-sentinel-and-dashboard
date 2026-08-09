class SentinelFileUtils
{
    static bool EnsureProfileDirectory()
    {
        Print(SentinelConstants.LOG_PREFIX + " profile directory check: " + SentinelConstants.PROFILE_DIR);

        if (!FileExist(SentinelConstants.PROFILE_DIR))
        {
            Print(SentinelConstants.LOG_PREFIX + " profile directory missing; creating it now");
            MakeDirectory(SentinelConstants.PROFILE_DIR);
        }

        if (!FileExist(SentinelConstants.PROFILE_DIR))
        {
            Print(SentinelConstants.LOG_PREFIX + " ERROR profile directory unavailable after MakeDirectory: " + SentinelConstants.PROFILE_DIR);
            return false;
        }

        Print(SentinelConstants.LOG_PREFIX + " profile directory ready: " + SentinelConstants.PROFILE_DIR);
        return true;
    }

    static void WriteEmptyArray(string path)
    {
        FileHandle file = OpenFile(path, FileMode.WRITE);

        if (file == 0)
        {
            Print(SentinelConstants.LOG_PREFIX + " empty file open failed: " + path);
            return;
        }

        FPrintln(file, "[]");
        CloseFile(file);
    }

    static string BoolToJson(bool value)
    {
        if (value)
        {
            return "true";
        }

        return "false";
    }
}
