
var keyboards = GetKeyboards();
var keyboard = null;
if(keyboards.length>0)
{
    keyboard = keyboards[0];
}

if(keyboard != null)
{
  keyboard.SetAllKeys(0x11,0xaa,0xff);
  //sleep(5000);
  //keyboard.SetAllKeys(0x11,0x22,0x11);


/*    for(var i=0;i<10;i++)
    {
        keyboard.SetAllKeys((i*20),(i*20),i*10);
        sleep(100);
    }
    for(var i=10;i>0;i--)
    {
        keyboard.SetAllKeys((i*20),(i*20),i*10);
        sleep(100);
    }
*/
}

for(var i=0;i<keyboards.length;i++)
{
    log(keyboards[i].ToString());
}
