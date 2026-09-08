import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class test {
    public static void main(String[] args) {
        String url = "jdbc:mysql://127.0.0.1:3306/?serverTimezone=Asia/Shanghai&characterEncoding=UTF-8";
        String user = "root";
        String password = "123456";

        try {
            // Use DriverManager to get a Connection
            Connection conn = DriverManager.getConnection(url, user, password);
            System.out.println("Connected to MySQL successfully.");
            
            Statement stmt = conn.createStatement();
            ResultSet rs = stmt.executeQuery("SELECT VERSION()");
            if (rs.next()) {
                System.out.println("MySQL version: " + rs.getString("VERSION()"));
            }
            rs.close();
            stmt.close();

            conn.close();
            System.out.println("Connection closed.");
        } catch (SQLException e) {
            System.out.println("Connection failed. Check MySQL service/user/password/timezone.");
            e.printStackTrace();
        }
    }
}
