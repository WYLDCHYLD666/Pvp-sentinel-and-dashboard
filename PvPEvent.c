
modded class PlayerBase{
 override void EEHitBy(TotalDamageResult d,int t,EntityAI src,int c,string z,string a,vector p,float s){
  super.EEHitBy(d,t,src,c,z,a,p,s);
  PlayerBase attacker=PlayerBase.Cast(src);
  if(attacker&&attacker!=this){ Print("[PvP] Illegal hit check"); }
 }}
