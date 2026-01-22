import matplotlib.pyplot as plt
import numpy as np

def is_valid_move(x, y, board):
    """检查移动是否有效且位置未被访问"""
    return 0 <= x < 8 and 0 <= y < 8 and board[x][y] == -1

def get_degree(x, y, board):
    """计算当前位置的度数（可到达的未访问位置数量）"""
    count = 0
    move_x = [2, 1, -1, -2, -2, -1, 1, 2]
    move_y = [1, 2, 2, 1, -1, -2, -2, -1]
    
    for i in range(8):
        if is_valid_move(x + move_x[i], y + move_y[i], board):
            count += 1
    return count

def is_closed_tour(board):
    """检查是否是闭合回路（起点和终点相邻）"""
    # 获取起点和终点位置
    start_pos = None
    end_pos = None
    for i in range(8):
        for j in range(8):
            if board[i][j] == 0:
                start_pos = (i, j)
            if board[i][j] == 63:
                end_pos = (i, j)
    
    if not start_pos or not end_pos:
        return False
    
    # 检查终点是否能一步跳到起点
    move_x = [2, 1, -1, -2, -2, -1, 1, 2]
    move_y = [1, 2, 2, 1, -1, -2, -2, -1]
    
    for i in range(8):
        if (end_pos[0] + move_x[i], end_pos[1] + move_y[i]) == start_pos:
            return True
    
    return False

def knights_tour_closed():
    """寻找马的哈密顿回路（闭合路径）"""
    # 初始化棋盘，-1表示未访问
    board = [[-1 for _ in range(8)] for _ in range(8)]
    
    # 马可能的8种移动方式
    move_x = [2, 1, -1, -2, -2, -1, 1, 2]
    move_y = [1, 2, 2, 1, -1, -2, -2, -1]
    
    # 从(0,0)开始
    board[0][0] = 0
    
    # 使用Warnsdorff规则的优化回溯法
    if not solve_knights_tour_closed(0, 0, 1, board, move_x, move_y):
        print("哈密顿回路不存在")
        return False, None
    else:
        print("找到马的哈密顿回路:")
        print_solution(board)
        
        # 获取路径坐标
        path = get_path(board)
        print("\n路径坐标顺序:")
        for i, (x, y) in enumerate(path):
            # 转换为1-based坐标（国际象棋标准表示法）
            print(f"({x+1}, {y+1})", end=" ")
            if (i+1) % 8 == 0:  # 每行显示8个坐标
                print()
        
        # 检查是否是闭合回路
        if is_closed_tour(board):
            print("\n这是一个闭合回路（哈密顿回路）")
        else:
            print("\n注意：这不是一个闭合回路")
        
        return True, board

def solve_knights_tour_closed(curr_x, curr_y, move_count, board, move_x, move_y):
    """使用Warnsdorff规则的优化回溯法，寻找哈密顿回路"""
    if move_count == 64:
        # 检查是否能形成闭合回路
        for i in range(8):
            next_x = curr_x + move_x[i]
            next_y = curr_y + move_y[i]
            if next_x == 0 and next_y == 0:  # 能回到起点
                return True
        return False
    
    # 获取所有可能的下一步移动
    next_moves = []
    for i in range(8):
        next_x = curr_x + move_x[i]
        next_y = curr_y + move_y[i]
        
        if is_valid_move(next_x, next_y, board):
            # 记录移动和其度数
            degree = get_degree(next_x, next_y, board)
            next_moves.append((next_x, next_y, degree))
    
    # 按度数排序（Warnsdorff规则：先选择度数最小的移动）
    next_moves.sort(key=lambda x: x[2])
    
    # 尝试所有可能的移动
    for move in next_moves:
        next_x, next_y, _ = move
        board[next_x][next_y] = move_count
        
        if solve_knights_tour_closed(next_x, next_y, move_count + 1, board, move_x, move_y):
            return True
        
        # 回溯
        board[next_x][next_y] = -1
    
    return False

def print_solution(board):
    """打印棋盘解决方案"""
    for i in range(8):
        for j in range(8):
            print(f"{board[i][j]:2d}", end=" ")
        print()

def get_path(board):
    """从棋盘状态获取路径坐标顺序"""
    path = [None] * 64
    for i in range(8):
        for j in range(8):
            step = board[i][j]
            path[step] = (i, j)
    return path

def visualize_knights_tour_closed(board):
    """可视化马的哈密顿回路"""
    fig, ax = plt.subplots(figsize=(10, 10))
    
    # 创建棋盘
    chessboard = np.zeros((8, 8))
    chessboard[1::2, ::2] = 1
    chessboard[::2, 1::2] = 1
    
    # 显示棋盘
    ax.imshow(chessboard, cmap='binary', origin='lower')
    
    # 获取路径
    path = get_path(board)
    
    # 绘制路径
    for step in range(63):
        x1, y1 = path[step]
        x2, y2 = path[step + 1]
        ax.plot([y1, y2], [x1, x2], 'b-', linewidth=2, alpha=0.7)
    
    # 如果是闭合回路，绘制从终点到起点的连线
    if is_closed_tour(board):
        start_x, start_y = path[0]
        end_x, end_y = path[63]
        ax.plot([end_y, start_y], [end_x, start_x], 'b-', linewidth=2, alpha=0.7, linestyle='--')
    
    # 标记起点和终点
    start_x, start_y = path[0]
    end_x, end_y = path[63]
    ax.plot(start_y, start_x, 'go', markersize=10, label='起点')
    ax.plot(end_y, end_x, 'ro', markersize=10, label='终点')
    
    # 添加步数标签
    for step, (x, y) in enumerate(path):
        ax.text(y, x, str(step), ha='center', va='center', fontsize=8, 
                bbox=dict(boxstyle="circle,pad=0.3", facecolor='white', alpha=0.7))
    
    ax.set_xticks(range(8))
    ax.set_yticks(range(8))
    ax.set_xticklabels(['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'])
    ax.set_yticklabels(range(1, 9))
    ax.grid(True)
    ax.legend()
    
    if is_closed_tour(board):
        ax.set_title('8×8棋盘上马的哈密顿回路')
    else:
        ax.set_title('8×8棋盘上马的哈密顿路径')
        
    plt.tight_layout()
    plt.show()

# 运行程序
if __name__ == "__main__":
    success, board = knights_tour_closed()
    
    if success:
        # 可视化路径
        visualize_knights_tour_closed(board)