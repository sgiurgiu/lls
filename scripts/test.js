
var keyboards = GetKeyboards();
var keyboard = null;
if(keyboards.length>0)
{
    keyboard = keyboards[0];
}

if(keyboard != null)
{
    
    for(var i=0;i<10;i++)
    {
        keyboard.SetAllKeys((i*20),(i*20),i*20);
        sleep(100);
    }
    for(var i=10;i>0;i--)
    {
        keyboard.SetAllKeys((i*20),(i*20),i*20);
        sleep(100);
    }
    
    keyboard.SetAllKeys(200,200,200);
}

for(var i=0;i<keyboards.length;i++)
{
    log(keyboards[i].ToString());
    log(JSON.stringify(keyboards[i].GetKeys()));
}
