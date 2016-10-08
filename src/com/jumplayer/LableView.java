package com.jumplayer;

import java.util.Vector;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.FontMetrics;
import android.util.AttributeSet;
import android.view.View;

public class LableView extends View
{
//	private Context	m_Context;
	
	private Paint	mPaint;
	private int 	mAscent;
	private String 	mStrText;
//	private int mTextPosx = 0;// x坐标
//	private int mTextPosy = 0;// y坐标
	private int mTextWidth = 0;// 绘制宽度
//	private int mTextHeight = 0;// 绘制高度
	private int mFontHeight = 0;// 绘制字体高度
	private int mRealLine = 0;// 字符串真实的行数
	private int mCurrentLine = 0;// 当前行
//	private int mTextSize = 0;// 字体大小
	private Vector<String> mString = null;
	public void setText(String szText)
	{
		mString.clear();// 清空Vector
		mStrText = szText;
		//GetTextIfon();
        requestLayout();
        invalidate();
	}
	
	public String getText()
	{
		return mStrText;
	}
	
	public LableView(Context context) 
	{
		super(context);
//		m_Context = context;
		initLabelView();
		
		mStrText = "";
	}

	public LableView(Context context,AttributeSet attr)
	{
		super(context,attr);
//		m_Context = context;
		initLabelView();
		
		mStrText = "";
	}

    private final void initLabelView() {
        mPaint = new Paint();
        mPaint.setAntiAlias(true);
        mPaint.setTextSize(18);
        mPaint.setColor(0xFFFFFFFF);
        mString = new Vector<String>();
        setPadding(3, 3, 3, 3);
    }

    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) 
    {
        setMeasuredDimension(measureWidth(widthMeasureSpec), measureHeight(heightMeasureSpec));
    }
 
    private int measureWidth(int measureSpec) 
    {
        int result = 0;
        int specMode = MeasureSpec.getMode(measureSpec);
        int specSize = MeasureSpec.getSize(measureSpec);
        
        if (specMode == MeasureSpec.EXACTLY) 
        {
            // We were told how big to be
            result = specSize;
        }
        else 
        {
            // Measure the text
            result = (int) mPaint.measureText(mStrText) + getPaddingLeft() + getPaddingRight();
            if (specMode == MeasureSpec.AT_MOST) 
            {
                // Respect AT_MOST value if that was what is called for by measureSpec
                result = Math.min(result, specSize);
            }
        }

        return result;
    }

    private int measureHeight(int measureSpec) 
    {
        int result = 0;
        int specMode = MeasureSpec.getMode(measureSpec);
        int specSize = MeasureSpec.getSize(measureSpec);
        
        mAscent = (int) mPaint.ascent();
        if (specMode == MeasureSpec.EXACTLY) 
        {
            // We were told how big to be
            result = specSize;
        } 
        else 
        {
            // Measure the text (beware: ascent is a negative number)
            result = (int) (-mAscent + mPaint.descent()) + getPaddingTop() + getPaddingBottom();
            if (specMode == MeasureSpec.AT_MOST) 
            {
                // Respect AT_MOST value if that was what is called for by measureSpec
                result = Math.min(result, specSize);
            }
        }
        return result;
    }
    public void GetTextIfon() {
		char ch;
		int w = 0;
		int istart = 0;
		FontMetrics fm = mPaint.getFontMetrics();// 得到系统默认字体属性
		mFontHeight = (int) (Math.ceil(fm.descent - fm.top) + 2);// 获得字体高度
		//mPageLineNum = mTextHeight / mFontHeight;// 获得行数
		int count = this.mStrText.length();
		for (int i = 0; i < count; i++) {
			ch = this.mStrText.charAt(i);
			float[] widths = new float[1];
			String str = String.valueOf(ch);
			mPaint.getTextWidths(str, widths);
			if (ch == '\n') {
				mRealLine++;// 真实的行数加一
				mString.addElement(this.mStrText.substring(istart, i));
				istart = i + 1;
				w = 0;
			} else {
				w += (int) Math.ceil(widths[0]);
				if (w > this.mTextWidth) {
					mRealLine++;// 真实的行数加一
					mString.addElement(this.mStrText.substring(istart, i));
					istart = i;
					i--;
					w = 0;
				} else {
					if (i == count - 1) {
						mRealLine++;// 真实的行数加一
						mString.addElement(this.mStrText.substring(istart,
								count));
					}
				}
			}
		}
	}
	protected void onDraw(Canvas canvas) 
	{
		// TODO Auto-generated method stub
		super.onDraw(canvas);
		
		this.mCurrentLine = this.mRealLine = 0;
		mTextWidth = canvas.getWidth();
//		mTextHeight= canvas.getHeight();
		
		GetTextIfon();
		
		
		for (int i = this.mCurrentLine; i < this.mRealLine; i++)
		{
			float x = getPaddingLeft();
			float y = getPaddingTop() - mAscent+this.mFontHeight * i;
			String str = (String) (mString.elementAt(i));
			canvas.drawText(str,x, y, mPaint);
		}
	}	
}
