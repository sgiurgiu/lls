
var keyboard = GetDefaultKeyboard();
if(keyboard != null)
{
    for (var i=0;i<20;i++)
    {
        keyboard.SetAllKeysColor(i*10,i*10,i*10);    
        sleep(100);
    }    

log(keyboard.ToString());
}

log(version());
