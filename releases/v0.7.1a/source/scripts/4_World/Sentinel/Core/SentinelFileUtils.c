class SentinelFileUtils
{
    static void EnsureProfileDirectory()
    {
        if (!FileExist(SentinelConstants.PROFILE_DIR))
        {
            MakeDirectory(SentinelConstants.PROFILE_DIR);
        }
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
