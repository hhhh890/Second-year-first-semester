# Mini-WeChat 💬

A database-centric, full-stack social platform implementation. This project recreates core WeChat functionalities with a heavy emphasis on backend integrity, role-based security, and efficient data modeling.

> **Academic Note**: This is the Final Project for the **CS145 Introduction to Database Systems** course at **Fudan University (Spring 2026)**, developed with my classmates **Lin Jiahui** and **Chen Ying** together.

---

## 🚀 Key Features

### 👤 Identity & Profile Management
- **Dual-Role Engine**: Fine-grained access control with distinct **User** and **Administrator** roles.
- **Dynamic Attributes**: Automated age calculation leveraging database **Triggers** to ensure synchronization between `birth_date` and `age` fields.
- **Data Sovereignty**: Strict application-level and database-level checks to guarantee users can only modify their own profiles.

### 👥 Sophisticated Social Graph
- **Symmetric Friendship**: Robust friend addition/removal logic ensuring data consistency across both ends of the relationship.
- **Custom Categorization**: Users can organize contacts into **Friend Groups** (e.g., "Work", "University") for structured relationship management.
- **Smart Search**: High-performance lookup of friends by name within authorized scopes.

### 🌍 Moments & Interactive Feed
- **Global-Local Feed**: A seamless stream integrating personal updates with friends' moments.
- **Multilevel Interactions**: Support for nested comments and real-time updates of moment timestamps.
- **Full Lifecycle Control**: Owners maintain complete CRUD permissions over their posted content.

### 💬 Private Messaging (Instant Chat)
- **Immersive View**: A WeChat-inspired double-pane UI with instant friend switching and scrolling message history.
- **Visual Design**: Modern message bubbles with distinct styles for sent and received texts.
- **Integrity First**: Message history is tethered to user accounts via foreign keys with `ON DELETE CASCADE` for automated cleanup.

### 🛡️ Administrative Audit System
- **Secure Monitoring**: Aggregated system insights provided through the `Admin_Moment_View` database view, abstracting complex joins.
- **Governance Tools**: Capabilities to purge illicit content or deport violating users while maintaining structural integrity.

---

## 🛠️ Technical Highlights

1.  **Logic at the Core**: Extensive use of **SQL Triggers** and **Views** to move business logic from the application layer to the database, enhancing performance and consistency.
2.  **Granular Security**: Implementation of MySQL **Roles and Privileges** to enforce the principle of least privilege, preventing unauthorized data access.
3.  **ACID Compliance**: Strategic use of **Foreign Key Constraints** and **Transactions** to prevent "orphan" data and ensure zero-corruption during bulk deletes.
4.  **Modern UX**: Built with **Streamlit**, delivering a high-performance web interface with minimal overhead.

---

## 🏃 Getting Started

### Prerequisites
- Python 3.9+
- MySQL Server 8.0+

### Setup Instructions

1.  **Hydrate the Database**:
    Execute the schema script in your MySQL environment:
    ```sql
    source D:/path/to/project/db_scheme.sql;
    ```
2.  **Install Runtime Environment**:
    ```bash
    pip install -r requirements.txt
    ```
3.  **Launch Platform**:
    ```bash
    streamlit run web_main.py
    ```

---
---

# Mini-WeChat 💬 (中文说明)

这是一个以数据库为核心、全栈式的社交平台实现。本项目复刻了微信的核心社交功能，重点展示了后端数据完整性、基于角色的安全机制（RBAC）以及高效的数据建模能力。

> **课程背景**: 本项目为 **复旦大学 2026年春季学期《数据库引论 (CS145)》** 期末项目，与我的同学**Jiahui Lin**、**Ying Chen** 共同完成。

---

## 🚀 核心功能

### 👤 身份与档案管理
- **双角色引擎**：支持 **用户** 与 **管理员** 两种身份的角色隔离与独立权限控制。
- **动态属性**：利用数据库 **触发器 (Triggers)** 自动维护 `birth_date` 与 `age` 的数据对齐，实现年龄自动更新。
- **隐私保护**：通过数据库角色（Roles）与应用逻辑双层校验，确保用户仅能访问及修改自身数据。

### 👥 社交关系图谱
- **双向好友机制**：严密的添加与删除逻辑，确保在 Friendship 表中建立一致、对称的社交关系。
- **分组管理系统**：支持自定义 **好友分组**（如“工作”、“大学”），实现联系人的结构化管理。
- **多维度检索**：在授权权限内提供高效的好友信息模糊/精确查找。

### 🌍 朋友圈互动流
- **混合信息流**：无缝整合个人动态与好友动态的实时信息流展示。
- **多级互动**：支持状态发布时间自动重置、多用户嵌套评论及其关联展示。
- **内容主权**：发布者拥有对个人状态完整的增删改查权利。

### 💬 私信即时通讯 (New!)
- **沉浸式交互**：采用仿微信的双栏布局，左侧联系人一键切换，右侧聊天窗口滚动查看。
- **视觉化气泡**：现代化的聊天气泡设计，通过颜色与对齐方式清晰区分发送与接收状态。
- **逻辑级联**：基于外键级联 (`ON DELETE CASCADE`) 的消息管理，确保用户注销时聊天数据自动净空。

### 🛡️ 管理员审计系统
- **统一监控**：通过 `Admin_Moment_View` 数据库视图抽象复杂关联，为管理员提供全局内容监控。
- **强力管控**：具备强制下线违规动态或注销违规用户的权限，确保系统合规性。

---

## 🛠️ 技术亮点

1.  **重心下沉**：深度利用 **SQL 触发器** 与 **视图** 将业务逻辑下沉至存储层，大幅提升了系统的响应速度与数据一致性。
2.  **精细化鉴权**：通过 MySQL **原生 Role 机制** 实现权限控制，从数据库底层杜绝越权访问。
3.  **数据原子性**：结合策略性的 **外键约束** 与 **事务管理**，确保系统在执行大规模级联操作时数据零损坏、零冗余。
4.  **数据原子性**：结合策略性的 **外键约束** 与 **事务管理**，确保系统在执行大规模级联操作时数据零损坏、零冗余。
5.  **极简现代 UI**：基于 **Streamlit** 框架构建，以极低的代码成本实现了响应快、颜值高的交互式 Web 体验。

---

## 🏃 运行环境

### 环境要求
- Python 3.9+
- MySQL Server 8.0+

### 操作步骤

1.  **数据库初始化**:
    在 MySQL 客户端中导入架构文件：
    ```sql
    source D:/你的路径/project/db_scheme.sql;
    ```
2.  **安装依赖库**:
    ```bash
    pip install -r requirements.txt
    ```
3.  **启动应用**:
    ```bash
    streamlit run web_main.py
    ```