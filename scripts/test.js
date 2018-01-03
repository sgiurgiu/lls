
var keyboards = GetKeyboards();
var keyboard = null;
if(keyboards.length>0)
{
    keyboard = keyboards[0];
}

if(keyboard != null)
{
    var keys = keyboard.GetKeys();
    for(var i=0;i<10;i++)
    {
        keyboard.UpdateKeys(keys, (i*20),(i*20),i*20);
        sleep(100);
    }
    for(var i=10;i>0;i--)
    {
        keyboard.UpdateKeys(keys, 0xff*i);
        sleep(100);
    }
    
    var keys = keyboard.GetKeys();
    keyboard.UpdateKeys(keys,0,0,0);
    
    for(var i=0;i<keys.length;i++)
    {
        keys[i].color = 0xffff11;
        keyboard.UpdateKey(keys[i]);
        sleep(100);
        log(JSON.stringify(keys[i]));
    }
    
}

for(var i=0;i<keyboards.length;i++)
{
    log(keyboards[i].ToString());
    log(JSON.stringify(keyboards[i].GetKeys()));
}
