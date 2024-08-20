package dependence.edge;

import dependence.vo.CGMethodVO;

public class MethodCall {
    private CGMethodVO callerMethod;
    private CGMethodVO calledMethod;
    private int lineNumber;

    public MethodCall(CGMethodVO callerMethod, CGMethodVO calledMethod, int lineNumber) {
        this.callerMethod = callerMethod;
        this.calledMethod = calledMethod;
        this.lineNumber = lineNumber;
    }

    public CGMethodVO getCallerMethod() {
        return callerMethod;
    }

    public void setCallerMethod(CGMethodVO callerMethod) {
        this.callerMethod = callerMethod;
    }

    public CGMethodVO getCalledMethod() {
        return calledMethod;
    }

    public void setCalledMethod(CGMethodVO calledMethod) {
        this.calledMethod = calledMethod;
    }

    public int getLineNumber() {
        return lineNumber;
    }

    public void setLineNumber(int lineNumber) {
        this.lineNumber = lineNumber;
    }

    public String getCallerMethodSig() {
        return callerMethod.getSig();
    }

    public String getCalledMethodSig() {
        return calledMethod.getSig();
    }

    @Override
    public String toString() {
        return "method call [" + callerMethod.getSig() + " -> " + calledMethod.getSig() + "] line: " + lineNumber;
    }

}
