def read(filePath, twoDArray):
	with open(filePath) as fp:
		line = fp.readline();
		cnt = 0;
		while line:
			list = [int(s) for s in line.split() if s.isdigit() ]
			# print('line {}: {}'.format(cnt, line.strip() ) )
			# print('row idx:{} col idx:{} byte sz:{}'.format(list[0],list[1],list[2]) );
			update2D(list[0], list[1], twoDArray, list[2]);

			line = fp.readline()
			cnt += 1	

def create2D(row, col):
	twoDArray = [[0 for x in range(row)] for y in range(col)]
	return twoDArray;	

def update2D(rowIdx, colIdx, twoDArray, val):
	# print('row idx:{} col idx:{} byte sz:{}'.format(rowIdx, colIdx, val) );
	twoDArray[rowIdx][colIdx] += val
	# print(twoDArray[rowIdx][colIdx])	

def print2D(twoDArray):
	rowIdx = 0 
	for r in twoDArray:
		colIdx = 0
		for c in r:
			print('2D[{}][{}]: {}'.format(rowIdx, colIdx, c) )
			# print('2D:{} {}'.format(rowIdx, colIdx) )
			# print(c)
			colIdx += 1
		rowIdx += 1	

def main():
	fp = '/home/xtaint/Desktop/verify_2DArray_small_log_update_trace.txt'
	twoDArray = create2D(7,7)
	read(fp, twoDArray)
	print2D(twoDArray)

if __name__ == "__main__":
    main()