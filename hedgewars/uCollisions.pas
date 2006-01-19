(*
 * Hedgewars, a worms-like game
 * Copyright (c) 2005 Andrey Korotaev <unC0Rr@gmail.com>
 *
 * Distributed under the terms of the BSD-modified licence:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *)

unit uCollisions;
interface
uses uGears;
{$INCLUDE options.inc}
const cMaxGearArrayInd = 255;

type TDirection = record
                  dX, dY: integer
                  end;
     PGearArray = ^TGearArray;
     TGearArray = record
                  ar: array[0..cMaxGearArrayInd] of PGear;
                  Count: Longword
                  end;

procedure FillRoundInLand(X, Y, Radius: integer; Value: Longword);
procedure AddGearCI(Gear: PGear);
procedure DeleteCI(Gear: PGear);
function CheckGearsCollision(Gear: PGear): PGearArray;
function  HHTestCollisionYwithGear(Gear: PGear; Dir: integer): boolean;
function TestCollisionXwithGear(Gear: PGear; Dir: integer): boolean;
function TestCollisionYwithGear(Gear: PGear; Dir: integer): boolean;
function TestCollisionXwithXYShift(Gear: PGear; ShiftX, ShiftY: integer; Dir: integer): boolean;
function TestCollisionYwithXYShift(Gear: PGear; ShiftX, ShiftY: integer; Dir: integer): boolean;

implementation
uses uMisc, uConsts, uLand;

type TCollisionEntry = record
                       X, Y, Radius: integer;
                       cGear: PGear;
                       end;
                       
const MAXRECTSINDEX = 255;
var Count: Longword = 0;
    cinfos: array[0..MAXRECTSINDEX] of TCollisionEntry;
    ga: TGearArray;

procedure FillRoundInLand(X, Y, Radius: integer; Value: Longword);
var ty, tx: integer;
begin
for ty:= max(-Radius, -y) to min(radius, 1023 - y) do
    for tx:= max(0, round(x-radius*sqrt(1-sqr(ty/radius)))) to min(2047,round(x+radius*sqrt(1-sqr(ty/radius)))) do
        Land[ty + y, tx]:= Value;
end;

procedure AddGearCI(Gear: PGear);
begin
if Gear.CollIndex < High(Longword) then exit; 
TryDo(Count <= MAXRECTSINDEX, 'Collision rects array overflow', true);
with cinfos[Count] do
     begin
     X:= round(Gear.X);
     Y:= round(Gear.Y);
     Radius:= Gear.Radius;
     FillRoundInLand(X, Y, Radius, $FF);
     cGear:= Gear
     end;
Gear.CollIndex:= Count;
inc(Count)
end;

procedure DeleteCI(Gear: PGear);
begin
if Gear.CollIndex < Count then
   begin
   with cinfos[Gear.CollIndex] do FillRoundInLand(X, Y, Radius, 0);
   cinfos[Gear.CollIndex]:= cinfos[Pred(Count)];
   cinfos[Gear.CollIndex].cGear.CollIndex:= Gear.CollIndex;
   Gear.CollIndex:= High(Longword);
   dec(Count)
   end;
end;

function CheckGearsCollision(Gear: PGear): PGearArray;
var mx, my: integer;
    i: Longword;
begin
Result:= @ga;
ga.Count:= 0;
if Count = 0 then exit;
mx:= round(Gear.X);
my:= round(Gear.Y);

for i:= 0 to Pred(Count) do
   with cinfos[i] do
      if (Gear <> cGear) and
         (sqrt(sqr(mx - x) + sqr(my - y)) <= Radius + Gear.Radius) then
             begin
             ga.ar[ga.Count]:= cinfos[i].cGear;
             inc(ga.Count)
             end;
end;

function HHTestCollisionYwithGear(Gear: PGear; Dir: integer): boolean;
var x, y, i: integer;
begin
Result:= false;
y:= round(Gear.Y);
if Dir < 0 then y:= y - Gear.Radius
           else y:= y + Gear.Radius;
           
if ((y - Dir) and $FFFFFC00) = 0 then
   begin
   x:= round(Gear.X);
   if (((x - Gear.Radius) and $FFFFF800) = 0)and(Land[y - Dir, x - Gear.Radius] <> 0)
    or(((x + Gear.Radius) and $FFFFF800) = 0)and(Land[y - Dir, x + Gear.Radius] <> 0) then
      begin
      Result:= true;
      exit
      end
    end;

if (y and $FFFFFC00) = 0 then
   begin
   x:= round(Gear.X) - Gear.Radius + 1;
   i:= x + Gear.Radius * 2 - 2;
   repeat
     if (x and $FFFFF800) = 0 then Result:= Land[y, x]<>0;
     inc(x)
   until (x > i) or Result
   end
end;

function TestCollisionXwithGear(Gear: PGear; Dir: integer): boolean;
var x, y, i: integer;
begin
Result:= false;
x:= round(Gear.X);
if Dir < 0 then x:= x - Gear.Radius
           else x:= x + Gear.Radius;
if (x and $FFFFF800) = 0 then
   begin
   y:= round(Gear.Y) - Gear.Radius + 1; {*}
   i:= y + Gear.Radius * 2 - 2;         {*}
   repeat
     if (y and $FFFFFC00) = 0 then Result:= Land[y, x]<>0;
     inc(y)
   until (y > i) or Result;
   end
end;

function TestCollisionXwithXYShift(Gear: PGear; ShiftX, ShiftY: integer; Dir: integer): boolean;
begin
Gear.X:= Gear.X + ShiftX;
Gear.Y:= Gear.Y + ShiftY;
Result:= TestCollisionXwithGear(Gear, Dir);
Gear.X:= Gear.X - ShiftX;
Gear.Y:= Gear.Y - ShiftY
end;

function TestCollisionYwithGear(Gear: PGear; Dir: integer): boolean;
var x, y, i: integer;
begin
Result:= false;
y:= round(Gear.Y);
if Dir < 0 then y:= y - Gear.Radius
           else y:= y + Gear.Radius;
if (y and $FFFFFC00) = 0 then
   begin
   x:= round(Gear.X) - Gear.Radius + 1;    {*}
   i:= x + Gear.Radius * 2 - 2;            {*}
   repeat
     if (x and $FFFFF800) = 0 then Result:= Land[y, x]<>0;
     inc(x)
   until (x > i) or Result;
   end
end;

function TestCollisionYwithXYShift(Gear: PGear; ShiftX, ShiftY: integer; Dir: integer): boolean;
begin
Gear.X:= Gear.X + ShiftX;
Gear.Y:= Gear.Y + ShiftY;
Result:= TestCollisionYwithGear(Gear, Dir);
Gear.X:= Gear.X - ShiftX;
Gear.Y:= Gear.Y - ShiftY
end;

end.
