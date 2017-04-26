print( 'Using ' .. _VERSION )
-- Mouse enumeration
Mouse = {}
Mouse.Button = {}
Mouse.Button.Left = 1
Mouse.Button.Right = 2

-- Widget Base Class
-- This provides members and methods for all GUI objects that will be feed user input.
-- These objects usally represent an interface item, but in some rare cases may not have
-- any visual element at all.
-- More specialised classes will be derived from this one.
--
Widget = {}
Widget.__index = Widget

-- Some Syntactic Sugar, calling a Class table is like calling it's create method.
function Widget.__call( class, ...)
    print( ... )
    return class.Create(...)
end

-- Initialisation for all widgets
function Widget:init( x, y, w, h, name )
    print( 'Initialising ' .. name )
    self.name = name
    self.rect = GUIRectangle( x, y, w, h )
    self.key = ''
    self.hover = false
    self.children = {}
end

-- Normally these should do nothing unless overridden, however we're doing a demo here!
function Widget:OnAction()         print( self.name .. ' OnAction' )         end
function Widget:OnKeyPress()       print( self.name .. ' OnKeyPress' ) return self:OnAction() end
function Widget:OnClick()          print( self.name .. ' OnClick' )    return self:OnAction() end
function Widget:OnAlternateClick() print( self.name .. ' OnAlternateClick' ) end
function Widget:OnHover()          print( self.name .. ' OnHover' )          end
function Widget:OnHoverExit()      print( self.name .. ' OnHoverExit' )      end

-- Does this widget respond to this key? No? What about it's children?
function Widget:KeyPress( key )
    if key == self.key  then
        return self:OnKeyPress()
    else
        for k, v in pairs( self.children ) do
            if v:KeyPress( key ) then
                return true
            end
        end
    end
end

-- Check if a mouse click is inside the rectangle that is our widget.
-- If it is, see if there is a child ( which should fit inside ) that
-- is a more specific match for the click position.
-- Otherwise respond ourselves.
--
-- If the click is outside, ignore it.
--
function Widget:MouseClick( button, x, y )
    print( 'Mouse clicked', self.name, button, x, y )
    if self.rect:isInside( x, y ) then
        for k, v in pairs( self.children ) do
            if v:MouseClick( button, x, y ) then
                return true
            end
        end

        print( self.name .. ' No child handled click' )

        if button == Mouse.Button.Left then
            return self:OnClick()
        elseif button == Mouse.Button.Right then
            return self:OnAlternateClick()
        end
    end
end

function Widget:MouseMove( x, y )
    print( self.name .. ' Mouse moved', x, y )
    if self.rect:isInside( x, y ) then
        print( '  Inside ' .. self.name )
        if self.hover == false then
            self.hover = true
            self:OnHover()
        end
        for k, v in pairs( self.children ) do
            v:MouseMove( x, y )
        end
    else
        print( '  Outside ' .. self.name )
        if self.hover == true then
            self.hover = false
            self:OnHoverExit()
            for k, v in pairs( self.children ) do
                v:MouseLost()
            end
        end
    end
end

-- Simpler, faster, version of MouseMove that assumes
-- that as the mouse is outside the parent widget, it is
-- outside this widget, and therefor the mouse is lost,
-- and informs it's children.
--
function Widget:MouseLost()
    print( self.name .. ' has lost the mouse.' )
    if self.hover then
        self.hover = false
        self:OnHoverExit()
        for k, v in pairs( self.children ) do
            v:MouseLost()
        end
    end
end

-- Add a child widget.  Child widgets should fit inside their parents
-- for the mouse targetting to work correctly.
--
function Widget:AddChild( child )
    if type( child.MouseMove ) == 'function' then -- Very basic check
        table.insert( self.children, child )
        child.parent = self
    else
        error( 'Not a valid type of Widget!' )
    end
end

-- A Push Button Widget.
Button = {}
Button.__index = Button
Button.__base = Widget
setmetatable( Button, Widget )

function Button:init( x, y, text )
    Button.__base.init( self, x, y, 100, 50, 'Button ' .. text )
    self.text = text
end

-- Override default Widget methods to give default Button actions!
function Button:OnAction()
    print( self.name .. " I've been activated!" )
    return true -- Event handled
end

function Button:OnHover()
    print( self.name .. ' Light up' )
end

function Button:OnHoverExit()
    print( self.name .. ' Extinguished!' )
end

function Button.Create( x, y, text )
    local btn = {}
    setmetatable( btn, Button )
    btn:init( x, y, text )
    return btn
end

-- A dialog box widget to hold buttons.
--
DialogBox = {}
DialogBox.__index = DialogBox
DialogBox.__base = Widget
setmetatable( DialogBox, Widget )

function DialogBox.Create( x, y, w, h, title )
    local dlg = {}
    setmetatable( dlg, DialogBox )
    dlg:init( x, y, w, h, 'DialogBox ' .. title )
    return dlg
end


-------------------------------------------------------------------------------

function test()
    -- A dialog box with two buttons.

    mybox = DialogBox( 10, 10, 500, 300, '"Save Game?"' )
    ok = Button( 20, 150, 'Ok' )
    cancel = Button( 150, 150, 'Cancel' )
    mybox:AddChild( ok )
    mybox:AddChild( cancel )

    print ( 'User moves to outside dialog box' )
    mybox:MouseMove( 5, 5 )
    print( '-------------------------------------' )

    print ( 'User moves mouse over "ok"' )
    mybox:MouseMove( 22, 153 )
    print( '-------------------------------------' )

    print ( 'User moves to "cancel"' )
    mybox:MouseMove( 155, 156 )
    print( '-------------------------------------' )

    print ( 'User clicks left button' )
    mybox:MouseClick( Mouse.Button.Left, 156, 154 )
    print( '-------------------------------------' )

end
