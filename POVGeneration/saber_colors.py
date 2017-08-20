from bibliopixel.colors.gamma import APA102
from bibliopixel import colors

c_list = [
    colors.Red,
    colors.Orange,
    colors.Yellow,
    colors.Green,
    colors.Blue,
    colors.Purple,
]

for c in c_list:
    print('{{{}, {}, {}}}'.format(APA102.get(c[0]), APA102.get(c[1]), APA102.get(c[2])))
