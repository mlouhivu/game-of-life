from optparse import OptionParser
import numpy as np
import pylab as pl

parser = OptionParser(usage='%prog [options]',
                      version='%prog 1.10')
parser.add_option('-d', '--dimension', type='int', default=32,
                  help='Size of the board')
parser.add_option('-s', '--shape', type='string', default='cross',
                  help='Initial shape of the board')
parser.add_option('-n', '--niter', type='int', default=500,
                  help='Number of iterations')
parser.add_option('-c', '--checkpoint', type='int', default=100,
                  help='Output frequency')
parser.add_option('-v', '--visualise', action="store_true", default=False,
                  help='Plot image during execution')

def initialize(size, shape='cross'):
    if shape == 'random':
        board = np.random.rand(size, size).round(0).astype(int)
    elif shape == 'cross':
        board = np.zeros((size, size), int)
        board[size/2,:] = 1
        board[:,size/2] = 1
    else:
        raise NotImplementedError('Unknown initial shape')

    # Periodic boundary conditions
    board[0,:] = board[-1,:]
    board[:,0] = board[:,-1]
    return board

def update(board):
    # number of neighbours that each square has
    neighbours = np.zeros(board.shape)
    neighbours[1:, 1:] += board[:-1, :-1]
    neighbours[1:, :-1] += board[:-1, 1:]
    neighbours[:-1, 1:] += board[1:, :-1]
    neighbours[:-1, :-1] += board[1:, 1:]
    neighbours[:-1, :] += board[1:, :]
    neighbours[1:, :] += board[:-1, :]
    neighbours[:, :-1] += board[:, 1:]
    neighbours[:, 1:] += board[:, :-1]

    new_board = np.where(neighbours < 2, 0, board)
    new_board = np.where(neighbours > 3, 0, new_board)
    new_board = np.where(neighbours == 3, 1, new_board)

    # Periodic boundaries
    new_board[0,:] = new_board[-1,:]
    new_board[:,0] = new_board[:,-1]
    return new_board

# initialize board
opt, args = parser.parse_args()
board = initialize(opt.dimension, opt.shape)

if opt.visualise:
    pl.ion()
    pl.hold(False)
    pl.imshow(board, cmap = pl.cm.prism)

for iter in range(opt.niter):
    board = update(board)
    if not iter % opt.checkpoint:
        np.save("board-%04d" % iter, board)
    if opt.visualise:
        pl.imshow(board, cmap = pl.cm.prism)
        pl.title('Generation %d' % iter)
        pl.draw()

if opt.visualise:
    pl.ioff()
    pl.show()

