/*
 * FormVibratoPreset.cs
 * Copyright c 2010 kbinani
 *
 * This file is part of org.kbinani.cadencii.
 *
 * org.kbinani.cadencii is free software; you can redistribute it and/or
 * modify it under the terms of the GPLv3 License.
 *
 * org.kbinani.cadencii is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#if JAVA
package org.kbinani.cadencii;

//INCLUDE-SECTION IMPORT ../BuildJavaUI/src/org/kbinani/Cadencii/FormVibratoConfig.java

import java.util.*;
import org.kbinani.*;
import org.kbinani.apputil.*;
import org.kbinani.vsq.*;
import org.kbinani.windows.forms.*;
#else
using System;
using org.kbinani.apputil;
using org.kbinani.vsq;
using org.kbinani;
using org.kbinani.java.util;
using org.kbinani.windows.forms;

namespace org.kbinani.cadencii
{
    using BEventArgs = System.EventArgs;
    using boolean = System.Boolean;
#endif

#if JAVA
    public class FormVibratoPreset extends BDialog
#else
    public class FormVibratoPreset : BDialog
#endif
    {
        /// <summary>
        /// �v���r���[�̊e�O���t�ɂ����āC�㉺�ɒǉ�����}�[�W���̍���(�s�N�Z��)
        /// </summary>
        private const int MARGIN = 3;
        /// <summary>
        /// �O��T�C�Y�ύX���́C�t�H�[���̕�
        /// </summary>
        private static int mPreviousWidth = 527;
        /// <summary>
        /// �O��T�C�Y�ύX���́C�t�H�[���̍���
        /// </summary>
        private static int mPreviousHeight = 418;

        /// <summary>
        /// AppManager.editorConfig.AutoVibratoCustom����R�s�[���Ă����C
        /// �r�u���[�g�n���h���̃��X�g
        /// </summary>
        private Vector<VibratoHandle> mHandles;
        /// <summary>
        /// �I����Ԃ̃r�u���[�g�n���h��
        /// </summary>
        private VibratoHandle mSelected = null;
        /// <summary>
        /// Rate�J�[�u��`�悷��̂Ɏg���`���
        /// </summary>
        private LineGraphDrawer mDrawerRate = null;
        /// <summary>
        /// Depth�J�[�u��`�悷��̂Ɏg���`���
        /// </summary>
        private LineGraphDrawer mDrawerDepth = null;
        /// <summary>
        /// ���ʂƂ��ē�����s�b�`�x���h�J�[�u��`�悷��̂Ɏg���`���
        /// </summary>
        private LineGraphDrawer mDrawerResulting = null;

        private org.kbinani.windows.forms.BButton buttonRemove;
        private org.kbinani.windows.forms.BButton buttonAdd;
        private org.kbinani.windows.forms.BButton buttonUp;
        private org.kbinani.windows.forms.BLabel labelRate;
        private org.kbinani.windows.forms.BLabel labelDepth;
        private NumberTextBox textRate;
        private NumberTextBox textDepth;
        private BLabel labelPresets;
        private BPictureBox pictureRate;
        private BLabel labelRateCurve;
        private BLabel labelDepthCurve;
        private BPictureBox pictureDepth;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private BLabel labelResulting;
        private BPictureBox pictureResulting;
        private BGroupBox groupEdit;
        private BLabel labelName;
        private BTextBox textName;
        private BGroupBox groupPreview;
        private System.Windows.Forms.ListBox listPresets;
        private org.kbinani.windows.forms.BButton buttonDown;

        /// <summary>
        /// �R���X�g���N�^�D
        /// </summary>
        /// <param name="handles"></param>
        public FormVibratoPreset( Vector<VibratoHandle> handles )
        {
#if JAVA
            super();
            initialize();
#else
            InitializeComponent();
#endif
            applyLanguage();
            Util.applyFontRecurse( this, AppManager.editorConfig.getBaseFont() );
            this.setSize( mPreviousWidth, mPreviousHeight );
            registerEventHandlers();

            // �n���h���̃��X�g���N���[��
            mHandles = new Vector<VibratoHandle>();
            int size = handles.size();
            for ( int i = 0; i < size; i++ ) {
                mHandles.add( (VibratoHandle)handles.get( i ).clone() );
            }

            // �\����Ԃ��X�V
            updateStatus();
            if ( size > 0 ) {
                listPresets.SelectedIndex = 0;
            }
        }

        #region public methods
        /// <summary>
        /// �_�C�A���O�ɂ��ݒ茋�ʂ��擾���܂�
        /// </summary>
        /// <returns></returns>
        public Vector<VibratoHandle> getResult()
        {
            // iconID�𐮂���
            if ( mHandles == null ) {
                mHandles = new Vector<VibratoHandle>();
            }
            int size = mHandles.size();
            for ( int i = 0; i < size; i++ ) {
                mHandles.get( i ).IconID = "$0404" + PortUtil.toHexString( i + 1, 4 );
            }
            return mHandles;
        }
        #endregion

        #region event handlers
        void listPresets_SelectedIndexChanged( object sender, EventArgs e )
        {
            // �C���f�b�N�X���擾
            int index = listPresets.SelectedIndex;

            // �͈͊O�Ȃ�bailout
            if ( (index < 0) || (mHandles.size() <= index) ) {
                mSelected = null;
                return;
            }

            // �C�x���g�n���h�����ꎞ�I�Ɏ�菜��
            textDepth.TextChanged -= textDepth_TextChanged;
            textRate.TextChanged -= textRate_TextChanged;
            textName.TextChanged -= textName_TextChanged;

            // �e�N�X�g�{�b�N�X�ɒl�𔽉f
            mSelected = mHandles.get( index );
            textDepth.setText( mSelected.getStartDepth() + "" );
            textRate.setText( mSelected.getStartRate() + "" );
            textName.setText( mSelected.getCaption() );

            // �C�x���g�n���h�����ēo�^
            textDepth.TextChanged += textDepth_TextChanged;
            textRate.TextChanged += textRate_TextChanged;
            textName.TextChanged += textName_TextChanged;

            // �ĕ`��
            repaintPictures();
        }

        void textName_TextChanged( object sender, EventArgs e )
        {
            if ( mSelected == null ) {
                return;
            }

            mSelected.setCaption( textName.getText() );
            updateStatus();
        }

        void textRate_TextChanged( object sender, EventArgs e )
        {
            if ( mSelected == null ) {
                return;
            }

            int old = mSelected.getStartRate();
            int value = old;
            string str = textRate.getText();
            try {
                value = PortUtil.parseInt( str );
            } catch ( Exception ex ) {
                value = old;
            }
            if ( value < 0 ) {
                value = 0;
            }
            if ( 127 < value ) {
                value = 127;
            }
            mSelected.setStartRate( value );
            string nstr = value + "";
            if ( str != nstr ) {
                textRate.setText( nstr );
                textRate.SelectionStart = textRate.Text.Length;
            }

            repaintPictures();
        }

        void textDepth_TextChanged( object sender, EventArgs e )
        {
            if ( mSelected == null ) {
                return;
            }

            int old = mSelected.getStartDepth();
            int value = old;
            string str = textDepth.getText();
            try {
                value = PortUtil.parseInt( str );
            } catch ( Exception ex ) {
                value = old;
            }
            if ( value < 0 ) {
                value = 0;
            }
            if ( 127 < value ) {
                value = 127;
            }
            mSelected.setStartDepth( value );
            string nstr = value + "";
            if ( str != nstr ) {
                textDepth.setText( nstr );
                textDepth.SelectionStart = textDepth.Text.Length;
            }

            repaintPictures();
        }

        void buttonAdd_Click( object sender, EventArgs e )
        {
            // �ǉ����C
            VibratoHandle handle = new VibratoHandle();
            handle.setCaption( "No-Name" );
            mHandles.add( handle );
            listPresets.SelectedIndices.Clear();
            // �\�����f������
            updateStatus();
            // �ǉ������̂�I����Ԃɂ���
            listPresets.SelectedIndex = mHandles.size() - 1;
        }

        void buttonRemove_Click( object sender, EventArgs e )
        {
            int index = listPresets.SelectedIndex;
            if ( index < 0 || listPresets.Items.Count <= index ) {
                return;
            }

            mHandles.removeElementAt( index );
            updateStatus();
        }

        void handleUpDownButtonClick( object sender, EventArgs e )
        {
            // ���M���̃{�^���ɂ���āC�I���C���f�b�N�X�̑�����ς���
            int delta = 1;
            if ( sender == buttonUp ) {
                delta = -1;
            }

            // �ړ���̃C���f�b�N�X�́H
            int index = listPresets.SelectedIndex;
            int move_to = index + delta;

            // �͈͓����ǂ���
            if ( index < 0 ) {
                return;
            }
            if ( move_to < 0 || mHandles.size() <= move_to ) {
                // �͈͊O�Ȃ牽�����Ȃ�
                return;
            }

            // ����ւ���
            VibratoHandle buff = mHandles.get( index );
            mHandles.set( index, mHandles.get( move_to ) );
            mHandles.set( move_to, buff );

            // �I����Ԃ�ς���
            listPresets.SelectedIndices.Clear();
            updateStatus();
            listPresets.SelectedIndex = move_to;
        }

        void pictureResulting_Paint( object sender, System.Windows.Forms.PaintEventArgs e )
        {
            // �w�i��`��
            int raw_width = pictureResulting.getWidth();
            int raw_height = pictureResulting.getHeight();
            System.Drawing.Graphics g = e.Graphics;
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.Default;
            g.FillRectangle( System.Drawing.Brushes.LightGray, 0, 0, raw_width, raw_height );

            // �I�𒆂̃n���h�����擾
            VibratoHandle handle = mSelected;
            if ( handle == null ) {
                return;
            }

            // �`��̏���
            LineGraphDrawer d = getDrawerResulting();
            d.setGraphics( g );

            // �r�u���[�g�̃s�b�`�x���h���擾����C�e���[�^���擾
            int width = raw_width;
            int vib_length = 960;
            int tempo = 500000;
            double vib_seconds = tempo * 1e-6 / 480.0 * vib_length;
            // 480�N���b�N��0.5�b
            VsqFileEx vsq = new VsqFileEx( "Miku", 1, 4, 4, tempo );
            VibratoBPList list_rate = handle.getRateBP();
            VibratoBPList list_depth = handle.getDepthBP();
            int start_rate = handle.getStartRate();
            int start_depth = handle.getStartDepth();
            if ( list_rate == null ) {
                list_rate = new VibratoBPList( new float[] { 0.0f }, new int[] { start_rate } );
            }
            if ( list_depth == null ) {
                list_depth = new VibratoBPList( new float[] { 0.0f }, new int[] { start_depth } );
            }
            // �𑜓x
            float resol = (float)(vib_seconds / width);
            if ( resol <= 0.0f ) {
                return;
            }
            VibratoPointIteratorBySec itr =
                new VibratoPointIteratorBySec(
                    vsq,
                    list_rate, start_rate,
                    list_depth, start_depth,
                    0, vib_length, resol );

            // �`��
            int height = raw_height - MARGIN * 2;
            d.clear();
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
            int x = 0;
            int lastx = 0;
            int lasty = -10;
            int tx = 0, ty = 0;
            const int MIN_DELTA = 2;
            for ( ; itr.hasNext(); x++ ) {
                double pitch = itr.next().getY();
                int y = height - (int)((pitch + 1.25) / 2.5 * height) + MARGIN - 1;
                int dx = x - lastx; // x�͒P������
                int dy = Math.Abs( y - lasty );
                tx = x;
                ty = y;
                if ( dx > MIN_DELTA || dy > MIN_DELTA ) {
                    d.append( x, y );
                    lastx = x;
                    lasty = y;
                }
            }
            d.append( tx, ty );
            d.flush();
        }

        void pictureRate_Paint( object sender, System.Windows.Forms.PaintEventArgs e )
        {
            // �w�i��`��
            int width = pictureRate.getWidth();
            int height = pictureRate.getHeight();
            System.Drawing.Graphics g = e.Graphics;
            g.FillRectangle( System.Drawing.Brushes.LightGray, 0, 0, width, height );

            // �I�𒆂̃n���h�����擾
            VibratoHandle handle = mSelected;
            if( handle == null ) {
                return;
            }

            // �`��̏���
            LineGraphDrawer d = getDrawerRate();
            d.clear();
            d.setGraphics( g );
            drawVibratoCurve( 
                handle.getRateBP(), 
                handle.getStartRate(),
                d, 
                width, height );
        }

        void pictureDepth_Paint( object sender, System.Windows.Forms.PaintEventArgs e )
        {
            // �w�i��`��
            int width = pictureDepth.getWidth();
            int height = pictureDepth.getHeight();
            System.Drawing.Graphics g = e.Graphics;
            g.FillRectangle( System.Drawing.Brushes.LightGray, 0, 0, width, height );

            // �I�𒆂̃n���h�����擾
            VibratoHandle handle = mSelected;
            if ( handle == null ) {
                return;
            }

            // �`��̏���
            LineGraphDrawer d = getDrawerDepth();
            d.clear();
            d.setGraphics( g );
            drawVibratoCurve(
                handle.getDepthBP(),
                handle.getStartDepth(),
                d,
                width, height );
        }

        void FormVibratoPreset_Resize( object sender, EventArgs e )
        {
            if ( this.WindowState == System.Windows.Forms.FormWindowState.Normal ) {
                mPreviousWidth = this.getWidth();
                mPreviousHeight = this.getHeight();
            }
            repaintPictures();
        }
        #endregion

        #region helper methods
        /// <summary>
        /// �C�x���g�n���h����o�^���܂�
        /// </summary>
        private void registerEventHandlers()
        {
            listPresets.SelectedIndexChanged += new EventHandler( listPresets_SelectedIndexChanged );
            textDepth.TextChanged += new EventHandler( textDepth_TextChanged );
            textRate.TextChanged += new EventHandler( textRate_TextChanged );
            textName.TextChanged += new EventHandler( textName_TextChanged );
            buttonAdd.Click += new EventHandler( buttonAdd_Click );
            buttonRemove.Click += new EventHandler( buttonRemove_Click );
            buttonUp.Click += new EventHandler( handleUpDownButtonClick );
            buttonDown.Click += new EventHandler( handleUpDownButtonClick );

            pictureDepth.Paint += new System.Windows.Forms.PaintEventHandler( pictureDepth_Paint );
            pictureRate.Paint += new System.Windows.Forms.PaintEventHandler( pictureRate_Paint );
            pictureResulting.Paint += new System.Windows.Forms.PaintEventHandler( pictureResulting_Paint );

            this.Resize += new EventHandler( FormVibratoPreset_Resize );
        }

        private static string _( string id )
        {
            return Messaging.getMessage( id );
        }

        private void applyLanguage()
        {
            this.setTitle( _( "Vibrato preset" ) );

            labelPresets.setText( _( "List of vibrato preset" ) );

            groupEdit.setTitle( _( "Edit" ) );
            labelName.setText( _( "Name" ) );

            groupPreview.setTitle( _( "Preview" ) );
            labelDepthCurve.setText( _( "Depth curve" ) );
            labelRateCurve.setText( _( "Rate curve" ) );
            labelResulting.setText( _( "Resulting pitch bend" ) );

            buttonAdd.setText( _( "Add" ) );
            buttonRemove.setText( _( "Remove" ) );
            buttonUp.setText( _( "Up" ) );
            buttonDown.setText( _( "Down" ) );

            buttonOk.setText( _( "OK" ) );
            buttonCancel.setText( _( "Cancel" ) );
        }

        /// <summary>
        /// Rate, Depth, Resulting pitch�̊e�O���t�������`�悵�܂�
        /// </summary>
        private void repaintPictures()
        {
            pictureDepth.repaint();
            pictureRate.repaint();
            pictureResulting.repaint();
        }

        /// <summary>
        /// �r�u���[�g��Rate�܂���Depth�J�[�u���w�肵���T�C�Y�ŕ`�悵�܂�
        /// </summary>
        /// <param name="list">�`�悷��J�[�u</param>
        /// <param name="start_value"></param>
        /// <param name="drawer"></param>
        /// <param name="width"></param>
        /// <param name="height"></param>
        private void drawVibratoCurve( VibratoBPList list, int start_value, LineGraphDrawer drawer, int width, int height )
        {
            int size = 0;
            if ( list != null ) {
                size = list.getCount();
            }
            drawer.clear();
            drawer.setBaseLineY( height );
            int iy0 = height - (int)(start_value / 127.0 * height);
            drawer.append( 0, iy0 );
            int lasty = iy0;
            for ( int i = 0; i < size; i++ ) {
                VibratoBPPair p = list.getElement( i );
                int ix = (int)(p.X * width);
                int iy = height - (int)(p.Y / 127.0 * height);
                drawer.append( ix, iy );
                lasty = iy;
            }
            drawer.append( width + drawer.getDotSize() * 2, lasty );
            drawer.flush();
        }

        /// <summary>
        /// Rate�J�[�u��`�悷��̂Ɏg���`�����擾���܂�
        /// </summary>
        /// <returns></returns>
        private LineGraphDrawer getDrawerRate()
        {
            if ( mDrawerRate == null ) {
                mDrawerRate = new LineGraphDrawer( LineGraphDrawer.TYPE_STEP );
                mDrawerRate.setDotMode( LineGraphDrawer.DOTMODE_ALWAYS );
                mDrawerRate.setFillColor( PortUtil.CornflowerBlue );
            }
            return mDrawerRate;
        }

        /// <summary>
        /// Depth�J�[�u��`�悷��̂Ɏg���`�����擾���܂�
        /// </summary>
        /// <returns></returns>
        private LineGraphDrawer getDrawerDepth()
        {
            if ( mDrawerDepth == null ) {
                mDrawerDepth = new LineGraphDrawer( LineGraphDrawer.TYPE_STEP );
                mDrawerDepth.setDotMode( LineGraphDrawer.DOTMODE_ALWAYS );
                mDrawerDepth.setFillColor( PortUtil.CornflowerBlue );
            }
            return mDrawerDepth;
        }

        /// <summary>
        /// ���ʂƂ��ē�����s�b�`�x���h�J�[�u��`�悷��̂Ɏg���`�����擾���܂�
        /// </summary>
        /// <returns></returns>
        private LineGraphDrawer getDrawerResulting()
        {
            if ( mDrawerResulting == null ) {
                mDrawerResulting = new LineGraphDrawer( LineGraphDrawer.TYPE_LINEAR );
                mDrawerResulting.setDotMode( LineGraphDrawer.DOTMODE_NO );
                mDrawerResulting.setFill( false );
                mDrawerResulting.setLineWidth( 2 );
                mDrawerResulting.setLineColor( PortUtil.ForestGreen );
            }
            return mDrawerResulting;
        }

        /// <summary>
        /// ��ʂ̕\����Ԃ��X�V���܂�
        /// </summary>
        private void updateStatus()
        {
            int old_select = listPresets.SelectedIndex;
            listPresets.SelectedIndices.Clear();

            // �A�C�e���̌��ɉߕs��������ΐ��𐮂���
            int size = mHandles.size();
            int delta = size - listPresets.Items.Count;
#if DEBUG
            PortUtil.println( "FormVibratoPreset#updateStatus; delta=" + delta );
#endif
            if ( delta > 0 ) {
                for ( int i = 0; i < delta; i++ ) {
                    listPresets.Items.Add( "" );
                }
            } else if ( delta < 0 ) {
                for ( int i = 0; i < -delta; i++ ) {
                    listPresets.Items.RemoveAt( 0 );
                }
            }

            // �A�C�e�����X�V
            for ( int i = 0; i < size; i++ ) {
                VibratoHandle handle = mHandles.get( i );
                listPresets.Items[i] = handle.getCaption();
            }

            // �I����Ԃ𕜋A
            if ( size <= old_select ) {
                old_select = size - 1;
            }
            if ( old_select >= 0 ) {
                listPresets.SelectedIndex = old_select;
            }
        }
        #endregion

#if JAVA
        #region UI Impl for Java
        //INCLUDE-SECTION FIELD ../BuildJavaUI/src/org/kbinani/Cadencii/FormVibratoConfig.java
        //INCLUDE-SECTION METHOD ../BuildJavaUI/src/org/kbinani/Cadencii/FormVibratoConfig.java
        #endregion
#else
        #region UI Impl for C#
        /// <summary>
        /// �K�v�ȃf�U�C�i�ϐ��ł��B
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// �g�p���̃��\�[�X�����ׂăN���[���A�b�v���܂��B
        /// </summary>
        /// <param name="disposing">�}�l�[�W ���\�[�X���j�������ꍇ true�A�j������Ȃ��ꍇ�� false �ł��B</param>
        protected override void Dispose( boolean disposing )
        {
            if ( disposing && (components != null) ) {
                components.Dispose();
            }
            base.Dispose( disposing );
        }

        #region Windows �t�H�[�� �f�U�C�i�Ő������ꂽ�R�[�h

        /// <summary>
        /// �f�U�C�i �T�|�[�g�ɕK�v�ȃ��\�b�h�ł��B���̃��\�b�h�̓��e��
        /// �R�[�h �G�f�B�^�ŕύX���Ȃ��ł��������B
        /// </summary>
        private void InitializeComponent()
        {
            this.buttonCancel = new org.kbinani.windows.forms.BButton();
            this.buttonOk = new org.kbinani.windows.forms.BButton();
            this.buttonRemove = new org.kbinani.windows.forms.BButton();
            this.buttonAdd = new org.kbinani.windows.forms.BButton();
            this.buttonUp = new org.kbinani.windows.forms.BButton();
            this.buttonDown = new org.kbinani.windows.forms.BButton();
            this.labelRate = new org.kbinani.windows.forms.BLabel();
            this.labelDepth = new org.kbinani.windows.forms.BLabel();
            this.labelPresets = new org.kbinani.windows.forms.BLabel();
            this.pictureRate = new org.kbinani.windows.forms.BPictureBox();
            this.labelRateCurve = new org.kbinani.windows.forms.BLabel();
            this.labelDepthCurve = new org.kbinani.windows.forms.BLabel();
            this.pictureDepth = new org.kbinani.windows.forms.BPictureBox();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.labelResulting = new org.kbinani.windows.forms.BLabel();
            this.pictureResulting = new org.kbinani.windows.forms.BPictureBox();
            this.groupEdit = new org.kbinani.windows.forms.BGroupBox();
            this.textName = new org.kbinani.windows.forms.BTextBox();
            this.labelName = new org.kbinani.windows.forms.BLabel();
            this.groupPreview = new org.kbinani.windows.forms.BGroupBox();
            this.listPresets = new System.Windows.Forms.ListBox();
            this.textDepth = new org.kbinani.cadencii.NumberTextBox();
            this.textRate = new org.kbinani.cadencii.NumberTextBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureRate)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureDepth)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureResulting)).BeginInit();
            this.groupEdit.SuspendLayout();
            this.groupPreview.SuspendLayout();
            this.SuspendLayout();
            // 
            // buttonCancel
            // 
            this.buttonCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonCancel.Location = new System.Drawing.Point( 424, 345 );
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size( 75, 23 );
            this.buttonCancel.TabIndex = 11;
            this.buttonCancel.Text = "Cancel";
            this.buttonCancel.UseVisualStyleBackColor = true;
            // 
            // buttonOk
            // 
            this.buttonOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonOk.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.buttonOk.Location = new System.Drawing.Point( 343, 345 );
            this.buttonOk.Name = "buttonOk";
            this.buttonOk.Size = new System.Drawing.Size( 75, 23 );
            this.buttonOk.TabIndex = 10;
            this.buttonOk.Text = "OK";
            this.buttonOk.UseVisualStyleBackColor = true;
            // 
            // buttonRemove
            // 
            this.buttonRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonRemove.Location = new System.Drawing.Point( 12, 301 );
            this.buttonRemove.Name = "buttonRemove";
            this.buttonRemove.Size = new System.Drawing.Size( 75, 23 );
            this.buttonRemove.TabIndex = 3;
            this.buttonRemove.Text = "Remove";
            this.buttonRemove.UseVisualStyleBackColor = true;
            // 
            // buttonAdd
            // 
            this.buttonAdd.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonAdd.Location = new System.Drawing.Point( 12, 272 );
            this.buttonAdd.Name = "buttonAdd";
            this.buttonAdd.Size = new System.Drawing.Size( 75, 23 );
            this.buttonAdd.TabIndex = 2;
            this.buttonAdd.Text = "Add";
            this.buttonAdd.UseVisualStyleBackColor = true;
            // 
            // buttonUp
            // 
            this.buttonUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonUp.Location = new System.Drawing.Point( 102, 272 );
            this.buttonUp.Name = "buttonUp";
            this.buttonUp.Size = new System.Drawing.Size( 75, 23 );
            this.buttonUp.TabIndex = 4;
            this.buttonUp.Text = "Up";
            this.buttonUp.UseVisualStyleBackColor = true;
            // 
            // buttonDown
            // 
            this.buttonDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonDown.Location = new System.Drawing.Point( 102, 301 );
            this.buttonDown.Name = "buttonDown";
            this.buttonDown.Size = new System.Drawing.Size( 75, 23 );
            this.buttonDown.TabIndex = 5;
            this.buttonDown.Text = "Down";
            this.buttonDown.UseVisualStyleBackColor = true;
            // 
            // labelRate
            // 
            this.labelRate.AutoSize = true;
            this.labelRate.Location = new System.Drawing.Point( 10, 15 );
            this.labelRate.Name = "labelRate";
            this.labelRate.Size = new System.Drawing.Size( 54, 12 );
            this.labelRate.TabIndex = 129;
            this.labelRate.Text = "Start rate";
            // 
            // labelDepth
            // 
            this.labelDepth.AutoSize = true;
            this.labelDepth.Location = new System.Drawing.Point( 10, 40 );
            this.labelDepth.Name = "labelDepth";
            this.labelDepth.Size = new System.Drawing.Size( 62, 12 );
            this.labelDepth.TabIndex = 130;
            this.labelDepth.Text = "Start depth";
            // 
            // labelPresets
            // 
            this.labelPresets.AutoSize = true;
            this.labelPresets.Location = new System.Drawing.Point( 12, 15 );
            this.labelPresets.Name = "labelPresets";
            this.labelPresets.Size = new System.Drawing.Size( 113, 12 );
            this.labelPresets.TabIndex = 134;
            this.labelPresets.Text = "List of preset vibrato";
            // 
            // pictureRate
            // 
            this.pictureRate.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureRate.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureRate.Location = new System.Drawing.Point( 12, 20 );
            this.pictureRate.Name = "pictureRate";
            this.pictureRate.Size = new System.Drawing.Size( 133, 74 );
            this.pictureRate.TabIndex = 135;
            this.pictureRate.TabStop = false;
            // 
            // labelRateCurve
            // 
            this.labelRateCurve.AutoSize = true;
            this.labelRateCurve.Location = new System.Drawing.Point( 10, 5 );
            this.labelRateCurve.Name = "labelRateCurve";
            this.labelRateCurve.Size = new System.Drawing.Size( 61, 12 );
            this.labelRateCurve.TabIndex = 136;
            this.labelRateCurve.Text = "Rate curve";
            // 
            // labelDepthCurve
            // 
            this.labelDepthCurve.AutoSize = true;
            this.labelDepthCurve.Location = new System.Drawing.Point( 10, 5 );
            this.labelDepthCurve.Name = "labelDepthCurve";
            this.labelDepthCurve.Size = new System.Drawing.Size( 67, 12 );
            this.labelDepthCurve.TabIndex = 137;
            this.labelDepthCurve.Text = "Depth curve";
            // 
            // pictureDepth
            // 
            this.pictureDepth.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureDepth.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureDepth.Location = new System.Drawing.Point( 12, 20 );
            this.pictureDepth.Name = "pictureDepth";
            this.pictureDepth.Size = new System.Drawing.Size( 146, 73 );
            this.pictureDepth.TabIndex = 138;
            this.pictureDepth.TabStop = false;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.IsSplitterFixed = true;
            this.splitContainer1.Location = new System.Drawing.Point( 0, 0 );
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add( this.pictureRate );
            this.splitContainer1.Panel1.Controls.Add( this.labelRateCurve );
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add( this.pictureDepth );
            this.splitContainer1.Panel2.Controls.Add( this.labelDepthCurve );
            this.splitContainer1.Size = new System.Drawing.Size( 310, 97 );
            this.splitContainer1.SplitterDistance = 148;
            this.splitContainer1.SplitterWidth = 1;
            this.splitContainer1.TabIndex = 139;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.IsSplitterFixed = true;
            this.splitContainer2.Location = new System.Drawing.Point( 3, 15 );
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add( this.splitContainer1 );
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add( this.pictureResulting );
            this.splitContainer2.Panel2.Controls.Add( this.labelResulting );
            this.splitContainer2.Size = new System.Drawing.Size( 310, 195 );
            this.splitContainer2.SplitterDistance = 97;
            this.splitContainer2.SplitterWidth = 1;
            this.splitContainer2.TabIndex = 140;
            // 
            // labelResulting
            // 
            this.labelResulting.AutoSize = true;
            this.labelResulting.Location = new System.Drawing.Point( 10, 5 );
            this.labelResulting.Name = "labelResulting";
            this.labelResulting.Size = new System.Drawing.Size( 110, 12 );
            this.labelResulting.TabIndex = 137;
            this.labelResulting.Text = "Resulting pitch bend";
            // 
            // pictureResulting
            // 
            this.pictureResulting.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureResulting.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureResulting.Location = new System.Drawing.Point( 12, 20 );
            this.pictureResulting.Name = "pictureResulting";
            this.pictureResulting.Size = new System.Drawing.Size( 295, 71 );
            this.pictureResulting.TabIndex = 136;
            this.pictureResulting.TabStop = false;
            // 
            // groupEdit
            // 
            this.groupEdit.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupEdit.Controls.Add( this.textName );
            this.groupEdit.Controls.Add( this.labelName );
            this.groupEdit.Controls.Add( this.labelDepth );
            this.groupEdit.Controls.Add( this.textDepth );
            this.groupEdit.Controls.Add( this.textRate );
            this.groupEdit.Controls.Add( this.labelRate );
            this.groupEdit.Location = new System.Drawing.Point( 183, 15 );
            this.groupEdit.Name = "groupEdit";
            this.groupEdit.Size = new System.Drawing.Size( 316, 90 );
            this.groupEdit.TabIndex = 6;
            this.groupEdit.TabStop = false;
            this.groupEdit.Text = "Edit";
            // 
            // textName
            // 
            this.textName.Location = new System.Drawing.Point( 86, 62 );
            this.textName.Name = "textName";
            this.textName.Size = new System.Drawing.Size( 169, 19 );
            this.textName.TabIndex = 9;
            // 
            // labelName
            // 
            this.labelName.AutoSize = true;
            this.labelName.Location = new System.Drawing.Point( 10, 65 );
            this.labelName.Name = "labelName";
            this.labelName.Size = new System.Drawing.Size( 34, 12 );
            this.labelName.TabIndex = 133;
            this.labelName.Text = "Name";
            // 
            // groupPreview
            // 
            this.groupPreview.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupPreview.Controls.Add( this.splitContainer2 );
            this.groupPreview.Location = new System.Drawing.Point( 183, 111 );
            this.groupPreview.Name = "groupPreview";
            this.groupPreview.Size = new System.Drawing.Size( 316, 213 );
            this.groupPreview.TabIndex = 142;
            this.groupPreview.TabStop = false;
            this.groupPreview.Text = "Preview";
            // 
            // listPresets
            // 
            this.listPresets.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.listPresets.FormattingEnabled = true;
            this.listPresets.ItemHeight = 12;
            this.listPresets.Location = new System.Drawing.Point( 12, 30 );
            this.listPresets.Name = "listPresets";
            this.listPresets.Size = new System.Drawing.Size( 165, 232 );
            this.listPresets.TabIndex = 1;
            // 
            // textDepth
            // 
            this.textDepth.BackColor = System.Drawing.Color.FromArgb( ((int)(((byte)(240)))), ((int)(((byte)(128)))), ((int)(((byte)(128)))) );
            this.textDepth.ForeColor = System.Drawing.Color.White;
            this.textDepth.Location = new System.Drawing.Point( 86, 37 );
            this.textDepth.Name = "textDepth";
            this.textDepth.Size = new System.Drawing.Size( 72, 19 );
            this.textDepth.TabIndex = 8;
            this.textDepth.Type = org.kbinani.cadencii.NumberTextBox.ValueType.Integer;
            // 
            // textRate
            // 
            this.textRate.BackColor = System.Drawing.Color.FromArgb( ((int)(((byte)(240)))), ((int)(((byte)(128)))), ((int)(((byte)(128)))) );
            this.textRate.ForeColor = System.Drawing.Color.White;
            this.textRate.Location = new System.Drawing.Point( 86, 12 );
            this.textRate.Name = "textRate";
            this.textRate.Size = new System.Drawing.Size( 72, 19 );
            this.textRate.TabIndex = 7;
            this.textRate.Type = org.kbinani.cadencii.NumberTextBox.ValueType.Integer;
            // 
            // FormVibratoPreset
            // 
            this.AcceptButton = this.buttonOk;
            this.AutoScaleDimensions = new System.Drawing.SizeF( 96F, 96F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.CancelButton = this.buttonCancel;
            this.ClientSize = new System.Drawing.Size( 511, 380 );
            this.Controls.Add( this.listPresets );
            this.Controls.Add( this.groupPreview );
            this.Controls.Add( this.groupEdit );
            this.Controls.Add( this.labelPresets );
            this.Controls.Add( this.buttonRemove );
            this.Controls.Add( this.buttonAdd );
            this.Controls.Add( this.buttonUp );
            this.Controls.Add( this.buttonDown );
            this.Controls.Add( this.buttonOk );
            this.Controls.Add( this.buttonCancel );
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormVibratoPreset";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Vibrato preset";
            ((System.ComponentModel.ISupportInitialize)(this.pictureRate)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureDepth)).EndInit();
            this.splitContainer1.Panel1.ResumeLayout( false );
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout( false );
            this.splitContainer1.Panel2.PerformLayout();
            this.splitContainer1.ResumeLayout( false );
            this.splitContainer2.Panel1.ResumeLayout( false );
            this.splitContainer2.Panel2.ResumeLayout( false );
            this.splitContainer2.Panel2.PerformLayout();
            this.splitContainer2.ResumeLayout( false );
            ((System.ComponentModel.ISupportInitialize)(this.pictureResulting)).EndInit();
            this.groupEdit.ResumeLayout( false );
            this.groupEdit.PerformLayout();
            this.groupPreview.ResumeLayout( false );
            this.ResumeLayout( false );
            this.PerformLayout();

        }
        #endregion

        private BButton buttonCancel;
        private BButton buttonOk;
        #endregion
#endif
    }

#if !JAVA
}
#endif
