# Lua like thing




## syntax
---
```Lua


-- main:
-- sp_push 8
function main()

    varible = 2;
    --[[
        varmap["varible"] = -4
        push_num 2
        sp_set,varmap["varible"]
    ]]--

    varible2 = varible+3;
    --[[
        varmap["varible2"] = -4
        push_num 2
        sp_get,varmap["varible"]
        add_signed
        sp_set,varmap["varible2"]
    ]]--

    --sp_pop
    --ret
end


```