/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package apidemo;

/**
 *
 * @author root
 */
public class JSMessageExample {
    public int senderId;
    public int userId;
    public String data;
    
    public JSMessageExample(int senderId, int userId, String data)
    {
        this.senderId = senderId;
        this.userId = userId;
        this.data = data;
    }
}
