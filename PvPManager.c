
class PvPManager{
static bool IsWeekend(){int d,h,m;GetGame().GetWorld().GetDate(d,h,m);
 if(d==5&&h>=23)return true; if(d==6)return true; if(d==0&&h<23)return true; return false;}}
