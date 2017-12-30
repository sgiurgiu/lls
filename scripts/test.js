
var keyboards = GetKeyboards();
for(var i=0;i<keyboards.length;i++)
{
    log(keyboards[i].ToString());
    keyboards[i].SetAllKeys(200,10,10);
}
