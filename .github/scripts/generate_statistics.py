import os
import json
import datetime
from pathlib import Path

 # 定义目录优先级顺序
    priority_dirs = ['include', 'docs', 'examples', 'bin', 'output', 'midi', 'test']
    
    def build_tree(path, prefix="", is_last=True, is_root=False):
        name = os.path.basename(path)
        
        # 如果是根目录，不显示名称
        if is_root:
            name = ""
        
        if os.path.isdir(path):
            if not is_root:  # 根目录不显示
                structure.append(f"{prefix}{'└── ' if is_last else '├── '}{name}/")
            
            # 获取目录内容并排序
            items = []
            for item in os.listdir(path):
                item_path = os.path.join(path, item)
                if not item.startswith('.') and item not in ['.git', '__pycache__', '.github']:
                    items.append((item, item_path, os.path.isdir(item_path)))
            
            # 自定义排序：优先目录在前，然后按优先级排序
            def custom_sort(item):
                name, _, is_dir = item
                # 目录优先
                if not is_dir:
                    return (1, name)
                # 按优先级排序
                if name in priority_dirs:
                    return (0, priority_dirs.index(name))
                # 其他目录按字母顺序
                return (0, len(priority_dirs) + ord(name[0]) if name else 0)
            
            items.sort(key=custom_sort)
            
            new_prefix = prefix + ("    " if is_last else "│   ")
            for i, (item_name, item_path, is_dir) in enumerate(items):
                build_tree(item_path, new_prefix, i == len(items) - 1, False)
        else:
            if not is_root:  # 根目录的文件不显示
                structure.append(f"{prefix}{'└── ' if is_last else '├── '}{name}")
    
    build_tree(root_dir, "", True, True)
    return structure

def get_file_statistics(root_dir):
    """获取文件统计信息"""
    stats = {
        'total_files': 0,
        'total_dirs': 0,
        'file_types': {},
        'total_size': 0
    }
    
    for root, dirs, files in os.walk(root_dir):
        # 过滤掉隐藏文件和.git目录
        dirs[:] = [d for d in dirs if not d.startswith('.') and d != '.git']
        files = [f for f in files if not f.startswith('.')]
        
        stats['total_dirs'] += len(dirs)
        stats['total_files'] += len(files)
        
        for file in files:
            file_path = os.path.join(root, file)
            try:
                file_size = os.path.getsize(file_path)
                stats['total_size'] += file_size
                
                # 统计文件类型
                ext = Path(file).suffix.lower()
                if ext:
                    stats['file_types'][ext] = stats['file_types'].get(ext, 0) + 1
                else:
                    stats['file_types']['no_extension'] = stats['file_types'].get('no_extension', 0) + 1
            except OSError:
                pass
    
    return stats

def get_development_time():
    """计算开发时间"""
    start_date = datetime.datetime(2020, 4, 26)
    current_date = datetime.datetime.now()
    
    
    return {
        'start_date': start_date.strftime('%Y/%m/%d'),
        'current_date': current_date.strftime('%Y/%m/%d'),
        'total_days': total_days,
    }

def main():
    root_dir = "."
    
    # 获取项目结构
    structure = get_project_structure(root_dir)
    
    # 获取文件统计
    stats = get_file_statistics(root_dir)
    
    # 获取开发时间
    dev_time = get_development_time()
    
    # 生成markdown内容
    md_content = f"""# 项目统计信息

## 项目基本信息
- **项目名称**: MidiParse
- **版本**: v7
- **开发时间**: {dev_time['start_date']} - {dev_time['current_date']}

## 项目结构
```
{chr(10).join(structure)}
```

## 文件统计
- **总文件数**: {stats['total_files']} 个
- **总目录数**: {stats['total_dirs']} 个
- **总大小**: {stats['total_size'] / 1024 / 1024:.2f} MB

### 文件类型分布
"""
    
    # 添加文件类型统计
    for ext, count in sorted(stats['file_types'].items(), key=lambda x: x[1], reverse=True):
        if ext == 'no_extension':
            md_content += f"- **无扩展名文件**: {count} 个\n"
        else:
            md_content += f"- **{ext}文件**: {count} 个\n"
    
    md_content += f"""
## 最近更新
_统计信息最后更新于: {dev_time['current_date']}_

---
*本文件由GitHub Actions自动生成*
"""
    
    # 写入statistics.md文件
    with open('statistics.md', 'w', encoding='utf-8') as f:
        f.write(md_content)
    
    print("统计信息已成功生成到 statistics.md")

if __name__ == "__main__":
    main()