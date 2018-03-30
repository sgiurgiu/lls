
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

    
    
    keyboard.UpdateKeys(keys,0x39,0xcc,0xcc);
    
    var keys = keyboard.GetKeysMatrix();
    for(var i =0;i<keys.length;i++)
    {
        log('length of row:'+i+' is:'+keys[i].length);
    }
    
}

for(var i=0;i<keyboards.length;i++)
{
    log(keyboards[i].ToString());
    log(JSON.stringify(keyboards[i].GetKeys()));
}
