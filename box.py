line = ' -- -- -- -- -- -- -- --'

for i in range(8):
	print line
	nums = []
	for j in range(8):
		num = (7-i)+(8*j)
		nums.append(num)
	print '|', '|'.join(str(x) for x in nums), '|'
print line
